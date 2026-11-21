#include <iostream>
#include <string>
#include <sstream>
#include "Workspace.h"

int main() {
    std::string command;
    Workspace workspace;
    std::string userName;
    std::string userEmail;

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
            else if (command == "git config" || command.rfind("git config ", 0) == 0) {
                const std::string usage =
                    "Usage: git config user.name|user.email \"<value>\"";

                std::istringstream input(command);
                std::string git, config, key, value, extra;
                char quote;

                // Read: git config user.name
                input >> git >> config >> key;
                if (key != "user.name" && key != "user.email") {
                    throw usage;
                }

                // Read the opening quote. >> skips spaces before it.
                if (!(input >> quote) || quote != '"') {
                    throw usage;
                }

                // Read the whole value, including spaces, up to the closing quote.
                std::getline(input, value, '"');
                if (input.eof() || value.empty()) {
                    throw usage;
                }

                if (input >> extra) {
                    throw usage;
                }

                if (key == "user.name") {
                    userName = value;
                } else {
                    userEmail = value;
                }
            }
            else if (command == "git" || command.rfind("git ", 0) == 0) {
                std::istringstream input(command);
                std::string git, subcommand, extra;
                input >> git >> subcommand;

                if (subcommand == "init") {
                    if (input >> extra) {
                        throw std::string("Usage: git init");
                    }

                    workspace.initRepository();
                } else if (!workspace.hasRepository()) {
                    throw std::string("Not a git repository");
                } else if (subcommand == "add") {
                    std::string name;

                    if (!(input >> name) || (input >> extra)) {
                        throw std::string("Usage: git add <file-name>|.");
                    }

                    workspace.add(name);
                } else if (subcommand == "reset") {
                    std::string name;

                    if (!(input >> name) || (input >> extra)) {
                        throw std::string("Usage: git reset <file-name>");
                    }

                    workspace.reset(name);
                } else if (subcommand == "commit") {
                    std::string option;
                    const std::string usage = "Usage: git commit -m \"<message>\"";

                    if (!(input >> option) || option != "-m") {
                        throw usage;
                    }

                    input >> std::ws;
                    if (input.get() != '"') {
                        throw usage;
                    }

                    std::string message;
                    if (!std::getline(input, message, '"') || input.eof() ||
                        message.empty() || (input >> extra)) {
                        throw usage;
                    }

                    workspace.commit(message, userName, userEmail);
                } else if (subcommand == "status") {
                    if (input >> extra) {
                        throw std::string("Usage: git status");
                    }

                    workspace.status();
                } else if (subcommand == "log") {
                    if (input >> extra) {
                        throw std::string("Usage: git log");
                    }

                    workspace.log();
                } else if (subcommand == "branch") {
                    const std::string usage = "Usage: git branch [<name> | -d <name>]";
                    std::string argument;

                    if (!(input >> argument)) {
                        workspace.listBranches();
                    } else if (argument == "-d") {
                        std::string name;
                        if (!(input >> name) || (input >> extra)) {
                            throw usage;
                        }
                        workspace.deleteBranch(name);
                    } else {
                        if (input >> extra) {
                            throw usage;
                        }
                        workspace.createBranch(argument);
                    }
                } else if (subcommand == "checkout") {
                    std::string target, extraArgument;
                    if (!(input >> target) || (input >> extraArgument)) {
                        throw std::string("Usage: git checkout <branch-name>");
                    }

                    workspace.checkout(target);
                } else {
                    throw std::string("Unknown command: " + command);
                }
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