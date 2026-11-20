#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <string>
#include <vector>
#include "Directory.h"

class Workspace {
private:
    Directory root;
    Directory* current;

    struct Repository {
        Directory* root;
        std::string branchName;
    };

    std::vector<Repository> repositories;

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
};

#endif
