#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <string>
#include <vector>
#include "Directory.h"

class Workspace {
private:
    Directory root;
    Directory* current;

    struct StagedFile {
        std::string path;
        std::vector<std::string> lines;
    };

    struct Commit {
        int id;
        int parentId; // 0 means there is no parent
        std::string message;
        std::string authorName;
        std::string authorEmail;
        std::string date;
        std::vector<StagedFile> snapshot;
    };

    struct Repository {
        Directory* root;
        std::string branchName;
        std::vector<StagedFile> staging;
        std::vector<Commit> commits;
    };

    std::vector<Repository> repositories;
    int nextCommitId = 1;

    Repository& currentRepository();

    static bool sameSnapshot(const std::vector<StagedFile>& left,
                             const std::vector<StagedFile>& right);

    std::string relativeDirectoryPath(const Directory* directory,
                                      const Directory* repositoryRoot) const;

    void collectFiles(const Directory* directory,
                      const std::string& prefix,
                      std::vector<StagedFile>& files) const;

public:
    Workspace();

    std::string pwd() const;
    std::string ls() const;
    void mkdir(const std::string& name);
    void touch(const std::string& name);
    void cd(const std::string& path);
    void echo(const std::string& text, const std::string& name, bool append);
    std::string cat(const std::string& name) const;
    void rm(const std::string& name);

    bool hasRepository() const;
    void initRepository();
    void add(const std::string& name);
    void reset(const std::string& name);
    void commit(const std::string& message,
                const std::string& authorName,
                const std::string& authorEmail);
};

#endif
