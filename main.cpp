#include <iostream>
#include <string>
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
