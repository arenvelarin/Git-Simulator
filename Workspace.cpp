#include "Workspace.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
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

void Workspace::echo(const std::string& text,
                     const std::string& name,
                     bool append) {
    touch(name);

    for (File& file : current->files) {
        if (file.name == name) {
            if (!append) {
                file.lines.clear();
            }

            file.lines.push_back(text);
            return;
        }
    }
}

std::string Workspace::cat(const std::string& name) const {
    for (const File& file : current->files) {
        if (file.name == name) {
            std::string result;
            for (const std::string& line : file.lines) {
                result += line + "\n";
            }
            return result;
        }
    }

    throw std::string("File not found: ") + name;
}

void Workspace::rm(const std::string& name) {
    for (auto it = current->files.begin(); it != current->files.end(); ++it) {
        if (it->name == name) {
            current->files.erase(it);
            return;
        }
    }

    for (const Directory* directory : current->directories) {
        if (directory->name == name) {
            throw std::string("Cannot remove a directory: ") + name;
        }
    }

    throw std::string("File not found: ") + name;
}

bool Workspace::hasRepository() const {
    for (const Directory* directory = current;
         directory != nullptr;
         directory = directory->parent) {
        for (const Repository& repository : repositories) {
            if (repository.root == directory) {
                return true;
            }
        }
    }

    return false;
}

void Workspace::initRepository() {
    if (hasRepository()) {
        throw std::string("Already inside a git repository");
    }

    repositories.push_back(Repository{current, "main", {}, {}});
}

std::string Workspace::relativeDirectoryPath(
    const Directory* directory,
    const Directory* repositoryRoot) const {

    std::string path;

    while (directory != repositoryRoot) {
        path = directory->name + "/" + path;
        directory = directory->parent;
    }

    return path;
}

void Workspace::collectFiles(
    const Directory* directory,
    const std::string& prefix,
    std::vector<StagedFile>& files) const {

    for (const File& file : directory->files) {
        files.push_back(StagedFile{prefix + file.name, file.lines});
    }

    for (const Directory* child : directory->directories) {
        collectFiles(child, prefix + child->name + "/", files);
    }
}

void Workspace::add(const std::string& name) {
    // Find the closest repository root, starting at the current directory.
    Repository* repository = nullptr;

    for (Directory* directory = current;
         directory != nullptr && repository == nullptr;
         directory = directory->parent) {

        for (Repository& candidate : repositories) {
            if (candidate.root == directory) {
                repository = &candidate;
                break;
            }
        }
    }

    if (repository == nullptr) {
        throw std::string("Not a git repository");
    }

    const std::string prefix = relativeDirectoryPath(current, repository->root);
    auto& staging = repository->staging;

    if (name == ".") {
        // Remove old entries only for the current directory and its children.
        for (auto it = staging.begin(); it != staging.end();) {
            if (it->path.rfind(prefix, 0) == 0) {
                it = staging.erase(it);
            } else {
                ++it;
            }
        }

        // Copy the files that still exist in this part of the workspace.
        collectFiles(current, prefix, staging);
        return;
    }

    if (name == ".." || name.find_first_of("/\\") != std::string::npos) {
        throw std::string("Usage: git add <file-name>|.");
    }

    const std::string path = prefix + name;

    // If the file exists, update its staged copy or add a new one.
    for (const File& file : current->files) {
        if (file.name == name) {
            for (StagedFile& stagedFile : staging) {
                if (stagedFile.path == path) {
                    stagedFile.lines = file.lines;
                    return;
                }
            }

            staging.push_back(StagedFile{path, file.lines});
            return;
        }
    }

    // If the workspace file was deleted, remove its staged entry.
    for (auto it = staging.begin(); it != staging.end(); ++it) {
        if (it->path == path) {
            staging.erase(it);
            return;
        }
    }

    throw std::string("File not found: " + name);
}

Workspace::Repository& Workspace::currentRepository() {
    for (Directory* directory = current;
         directory != nullptr;
         directory = directory->parent) {
        for (Repository& repository : repositories) {
            if (repository.root == directory) {
                return repository;
            }
        }
    }

    throw std::string("Not a git repository");
}

bool Workspace::sameSnapshot(const std::vector<StagedFile>& left,
                             const std::vector<StagedFile>& right) {
    if (left.size() != right.size()) {
        return false;
    }

    // Compare by path, not by position in the vector.
    for (const StagedFile& file : left) {
        bool found = false;

        for (const StagedFile& other : right) {
            if (file.path == other.path && file.lines == other.lines) {
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}

void Workspace::commit(const std::string& message,
                       const std::string& authorName,
                       const std::string& authorEmail) {
    Repository& repository = currentRepository();

    if (authorName.empty() || authorEmail.empty()) {
        throw std::string("Set user.name and user.email before committing");
    }

    const bool hasCommit = !repository.commits.empty();
    const std::vector<StagedFile> emptySnapshot;
    const auto& previous = hasCommit
        ? repository.commits.back().snapshot
        : emptySnapshot;

    if (sameSnapshot(repository.staging, previous)) {
        std::vector<StagedFile> workingFiles;
        collectFiles(repository.root, "", workingFiles);

        if (sameSnapshot(workingFiles, repository.staging)) {
            std::cout << "nothing to commit, working tree clean\n";
        } else {
            std::cout << "no changes added to commit\n";
        }

        return;
    }

    const int parentId = hasCommit ? repository.commits.back().id : 0;
    const std::time_t now = std::time(nullptr);
    const std::tm* localTime = std::localtime(&now);

    if (localTime == nullptr) {
        throw std::string("Could not read the commit time");
    }

    std::ostringstream date;
    date << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

    const int id = nextCommitId;
    repository.commits.push_back(Commit{
        id, parentId, message, authorName, authorEmail,
        date.str(), repository.staging
    });
    ++nextCommitId;

    std::cout << '[' << repository.branchName << ' ' << id
              << "] " << message << '\n';
}

void Workspace::reset(const std::string& name) {
    if (name == "." || name == ".." ||
        name.find_first_of("/\\") != std::string::npos) {
        throw std::string("Usage: git reset <file-name>");
    }

    Repository& repository = currentRepository();
    const std::string path = relativeDirectoryPath(current, repository.root) + name;

    const StagedFile* committedFile = nullptr;
    if (!repository.commits.empty()) {
        for (const StagedFile& file : repository.commits.back().snapshot) {
            if (file.path == path) {
                committedFile = &file;
                break;
            }
        }
    }

    auto& staging = repository.staging;
    for (auto it = staging.begin(); it != staging.end(); ++it) {
        if (it->path == path) {
            if (committedFile != nullptr) {
                it->lines = committedFile->lines;
            } else {
                staging.erase(it);
            }
            return;
        }
    }

    if (committedFile != nullptr) {
        staging.push_back(*committedFile);
    }
}

void Workspace::status() {
    const Repository& repository = currentRepository();
    const std::vector<StagedFile> emptySnapshot;
    const auto& committed = repository.commits.empty()
        ? emptySnapshot
        : repository.commits.back().snapshot;
    const auto& staging = repository.staging;

    std::vector<StagedFile> working;
    collectFiles(repository.root, "", working);

    struct StatusEntry {
        std::string path;
        std::string label;
    };

    std::vector<StatusEntry> stagedChanges;
    std::vector<StatusEntry> unstagedChanges;
    std::vector<StatusEntry> untrackedFiles;

    auto findFile = [](const std::vector<StagedFile>& files,
                       const std::string& path) -> const StagedFile* {
        for (const StagedFile& file : files) {
            if (file.path == path) {
                return &file;
            }
        }
        return nullptr;
    };

    for (const StagedFile& file : staging) {
        const StagedFile* oldFile = findFile(committed, file.path);
        if (oldFile == nullptr) {
            stagedChanges.push_back({file.path, "new file: "});
        } else if (oldFile->lines != file.lines) {
            stagedChanges.push_back({file.path, "modified: "});
        }

        const StagedFile* workingFile = findFile(working, file.path);
        if (workingFile == nullptr) {
            unstagedChanges.push_back({file.path, "deleted: "});
        } else if (workingFile->lines != file.lines) {
            unstagedChanges.push_back({file.path, "modified: "});
        }
    }

    for (const StagedFile& file : committed) {
        if (findFile(staging, file.path) == nullptr) {
            stagedChanges.push_back({file.path, "deleted: "});
        }
    }

    for (const StagedFile& file : working) {
        if (findFile(staging, file.path) == nullptr) {
            untrackedFiles.push_back({file.path, ""});
        }
    }

    auto printSection = [](const char* title, std::vector<StatusEntry>& entries) {
        if (entries.empty()) {
            return;
        }

        std::sort(entries.begin(), entries.end(),
                  [](const StatusEntry& a, const StatusEntry& b) {
                      return a.path < b.path;
                  });

        std::cout << '\n' << title << ":\n";
        for (const StatusEntry& entry : entries) {
            std::cout << "    " << entry.label << entry.path << '\n';
        }
    };

    std::cout << "On branch " << repository.branchName << '\n';

    if (stagedChanges.empty() && unstagedChanges.empty() &&
        untrackedFiles.empty()) {
        std::cout << "nothing to commit, working tree clean\n";
        return;
    }

    printSection("Staged changes", stagedChanges);
    printSection("Modified (not staged)", unstagedChanges);
    printSection("Untracked files", untrackedFiles);
}