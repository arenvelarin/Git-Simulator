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
};

#endif
