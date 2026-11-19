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
    std::vector<std::string> names;

    for (const Directory* directory : current->directories) {
        names.push_back(directory->name);
    }

    for (const File& file : current->files) {
        names.push_back(file.name);
    }

    for (std::size_t i = 1; i < names.size(); ++i) {
        std::string name = names[i];
        std::size_t j = i;

        while (j > 0 && name < names[j - 1]) {
            names[j] = names[j - 1];
            --j;
        }

        names[j] = name;
    }

    std::string result;
    for (const std::string& name : names) {
        result += name + "\n";
    }
    
    return result;
}