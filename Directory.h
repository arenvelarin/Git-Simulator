#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <vector>
#include "File.h"

struct Directory {
    std::string name;
    Directory* parent;

    std::vector<File> files;
    std::vector<Directory*> directories;

    Directory(const std::string& directoryName,
              Directory* parentDirectory)
        : name(directoryName), parent(parentDirectory) {
    }

    ~Directory() {
        for (Directory* directory : directories) {
            delete directory;
        }
    }

    Directory(const Directory&) = delete;
    Directory& operator=(const Directory&) = delete;
};

#endif
