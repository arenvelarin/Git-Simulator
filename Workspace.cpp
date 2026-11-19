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

std::string Workspace::ls() const {
    std::string result;

    for (const Directory* directory : current->directories) {
        result += directory->name + "/\n";
    }

    for (const File& file : current->files) {
        result += file.name + "\n";
    }

    return result;
}