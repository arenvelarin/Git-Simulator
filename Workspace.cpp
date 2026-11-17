#include "Workspace.h"

Workspace::Workspace()
    : root("/", nullptr), current(&root) {
}

std::string Workspace::pwd() const {
    if (current == &root) {
        return "/";
    }

    std::string path;
    const Directory* directory = current;

    while (directory != &root) {
        path = "/" + directory->name + path;
        directory = directory->parent;
    }

    return path;
}

