#include "Workspace.h"
#include <sstream>

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

void Workspace::touch(const std::string& name) {
    if (name.empty() || name == "." || name == ".." ||
        name.find('/') != std::string::npos) {
        throw std::string("Invalid file name");
    }

    for (const Directory* directory : current->directories) {
        if (directory->name == name) {
            throw std::string("Name already exists: ") + name;
        }
    }

    for (const File& file : current->files) {
        if (file.name == name) {
            return;
        }
    }

    current->files.push_back(File{name, {}});
}

void Workspace::cd(const std::string& path) {
    if (path.empty()) {
        throw std::string("Usage: cd <directory-path>");
    }

    Directory* target = current;

    // If the path starts with /, start from the root.
    if (path.front() == '/') {
        while (target->parent != nullptr) {
            target = target->parent;
        }
    }

    std::istringstream input(path);
    std::string part;

    while (std::getline(input, part, '/')) {
        if (part.empty() || part == ".") {
            continue;
        }

        if (part == "..") {
            if (target->parent != nullptr) {
                target = target->parent;
            }
            continue;
        }

        Directory* next = nullptr;
        for (Directory* directory : target->directories) {
            if (directory->name == part) {
                next = directory;
                break;
            }
        }

        if (next == nullptr) {
            throw std::string("Directory not found: ") + path;
        }

        target = next;
    }

    current = target;
}