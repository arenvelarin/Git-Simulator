#include <iostream>
#include <string>

int main() {
    std::string command;

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
