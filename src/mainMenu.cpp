#include "mainMenu.h"
#include <iostream>

mainMenu::mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper) {
    m_cfgWrapper = aConfigWrapper;
    clearConsole();
    displayMenu();
}

/*void mainMenu::displayMenu() {
    std::string choice;
    while (true) {
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓          MAIN MENU         ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓   1. show module by Alias  ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓   2. show module by Slot   ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓   X.       exit            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == "1") {
            // Execute code for Option 1
            std::cout << "You chose Option 1." << std::endl;
        } 
        else if (choice == "2") 
        {
            // Show module by Slot
            clearConsole();
            std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
            std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
            std::cout << "✩░▒▓        Choose the slot     ▓▒░✩" << std::endl;
            std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
            std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl; 
            std::cout << "Enter solt number: ";
            std::cin >> choice;
            
        } 
        else if (choice == "x" || choice == "X") 
        {
            std::cout << "Exiting..." << std::endl;
            break;
        } else {
            std::cout << "Invalid choice. Try again." << std::endl;
        }
    }
}*/


void mainMenu::displayMenu() {
    std::string choice;
    while (true) {
        clearConsole();
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓          MAIN MENU         ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓   1. show module by Alias  ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓   2. show module by Slot   ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓   X.       exit            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == "1") {
            std::cout << "You chose Option 1." << std::endl;
        } 
        else if (choice == "2") {
            clearConsole();
            std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
            std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
            std::cout << "✩░▒▓        Choose the slot     ▓▒░✩" << std::endl;
            std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
            std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl; 
            std::cout << "Enter slot number: ";
            std::cin >> choice;

            try {
                unsigned int slotNumber = std::stoi(choice);
                if (slotNumber >= 1 && slotNumber <= 6) {
                    auto module = m_cfgWrapper->getModuleBySlot(slotNumber);
                    module->showModuleOnConsole();
                    std::cout << "Press Enter to continue...";
                    std::cin.ignore();
                    std::cin.get();
                } else {
                    std::cout << "Invalid slot number. Must be between 1 and 6." << std::endl;
                }
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid input. Please enter a number between 1 and 6." << std::endl;
            }
        } 
        else if (choice == "x" || choice == "X") {
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
