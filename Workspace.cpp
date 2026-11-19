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

void Workspace::mkdir(const std::string& name) {
    if (name.empty() || name == "." || name == ".." ||
        name.find('/') != std::string::npos) {
        throw std::string("Invalid directory name");
    }

    for (const Directory* directory : current->directories) {
        if (directory->name == name) {
            throw std::string("Name already exists: ") + name;
        }
    }

    for (const File& file : current->files) {
        if (file.name == name) {
            throw std::string("Name already exists: ") + name;
        }
    }

    Directory* directory = new Directory(name, current);
    try {
        current->directories.push_back(directory);
    }
    catch (...) {
        delete directory;
        throw;
    }
}