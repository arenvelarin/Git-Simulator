#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <string>
#include "Directory.h"

class Workspace {
private:
    Directory root;
    Directory* current;

public:
    Workspace();

    std::string pwd() const;
    std::string ls() const;
    void mkdir(const std::string& name);
    void touch(const std::string& name);
    void cd(const std::string& path);
};

#endif
