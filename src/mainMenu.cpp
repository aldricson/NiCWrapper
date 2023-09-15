#include "mainMenu.h"
#include <iostream>

mainMenu::mainMenu() {
    clearConsole();
    displayMenu();
}

void mainMenu::displayMenu() {
    std::string choice;
    while (true) {
        std::cout << "Main Menu:" << std::endl;
        std::cout << "1. Option 1" << std::endl;
        std::cout << "2. Option 2" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == "1") {
            // Execute code for Option 1
            std::cout << "You chose Option 1." << std::endl;
        } else if (choice == "2") {
            // Execute code for Option 2
            std::cout << "You chose Option 2." << std::endl;
        } else if (choice == "3") {
            std::cout << "Exiting..." << std::endl;
            break;
        } else {
            std::cout << "Invalid choice. Try again." << std::endl;
        }
    }
}

void mainMenu::clearConsole()
{
    // ANSI escape sequence to clear screen for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
}
