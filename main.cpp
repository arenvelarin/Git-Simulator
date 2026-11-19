#include <iostream>
#include <string>
#include <sstream>
#include "Workspace.h"

int main() {
    std::string command;
    Workspace workspace;

    while (true) {
        std::cout << "$ ";

        if (!std::getline(std::cin, command)) {
            break;
        }

        try {
            if (command == "") {
                continue;
            }
            else if (command == "exit") {
                break;
            }
            else if (command == "pwd") {
                std::cout << workspace.pwd() << std::endl;
            }
            else if (command == "ls") {
                std::cout << workspace.ls();
            }
            else if (command == "mkdir" || command.rfind("mkdir ", 0) == 0) {
                std::istringstream input(command);
                std::string verb, name, extra;

                if (!(input >> verb >> name) || (input >> extra)) {
                    throw std::string("Usage: mkdir <directory-name>");
                }

                workspace.mkdir(name);
            }
            else if (command == "touch" || command.rfind("touch ", 0) == 0) {
                std::istringstream input(command);
                std::string verb, name, extra;

                if (!(input >> verb >> name) || (input >> extra)) {
                    throw std::string("Usage: touch <file-name>");
                }

                workspace.touch(name);
            }
            else if (command == "cd" || command.rfind("cd ", 0) == 0) {
                std::istringstream input(command);
                std::string verb, path, extra;

                if (!(input >> verb >> path) || (input >> extra)) {
                    throw std::string("Usage: cd <directory-path>");
                }

                workspace.cd(path);
            }
            else {
                throw std::string("Unknown command: " + command);
            }
        }
        catch (const std::string& error) {
            std::cout << "Error: " << error << std::endl;
        }
    }

    return 0;
}