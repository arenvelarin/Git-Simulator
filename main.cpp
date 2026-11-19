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
            if (command.empty()) {
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
            else if (command == "echo" || command.rfind("echo ", 0) == 0) {
                const std::string usage = "Usage: echo \"<text>\" >|>> <file-name>";

                // Skip whitespace after "echo" and check for the opening quote.
                std::size_t firstQuote = command.find_first_not_of(" \t\r\n", 4);
                if (firstQuote == std::string::npos || command[firstQuote] != '"') {
                    throw usage;
                }

                std::size_t secondQuote = command.find('"', firstQuote + 1);
                if (secondQuote == std::string::npos) {
                    throw usage;
                }

                std::string text = command.substr(firstQuote + 1,secondQuote - firstQuote - 1);

                // Find the operator after the closing quote.
                std::size_t op = command.find_first_not_of(" \t\r\n", secondQuote + 1);
                if (op == std::string::npos || command[op] != '>') {
                    throw usage;
                }

                bool append = command.compare(op, 2, ">>") == 0;
                std::size_t nameStart = op + (append ? 2 : 1);

                std::istringstream input(command.substr(nameStart));
                std::string name, extra;

                if (!(input >> name) || (input >> extra) ||
                    name.find_first_of("<>\"") != std::string::npos) {
                    throw usage;
                }

                workspace.echo(text, name, append);
            }
            else if (command == "cat" || command.rfind("cat ", 0) == 0) {
                std::istringstream input(command);
                std::string verb, name, extra;

                if (!(input >> verb >> name) || (input >> extra)) {
                    throw std::string("Usage: cat <file-name>");
                }

                std::cout << workspace.cat(name);
            }
            else if (command == "rm" || command.rfind("rm ", 0) == 0) {
                std::istringstream input(command);
                std::string verb, name, extra;

                if (!(input >> verb >> name) || (input >> extra)) {
                    throw std::string("Usage: rm <file-name>");
                }

                workspace.rm(name);
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
