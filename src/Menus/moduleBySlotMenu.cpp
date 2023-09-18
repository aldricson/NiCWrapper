#include "moduleBySlotMenu.h"
#include <iostream>

moduleBySlotMenu::moduleBySlotMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper) {
    m_cfgWrapper = aConfigWrapper;

}


/*void moduleBySlotMenu::displayMenu() 
{
    std::string choice;

    // Adding a loop to keep asking for input until a valid one is entered
    while (true) 
    {
        clearConsole();
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓        Choose the slot     ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓  X -> return to main menu  ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl; 
        std::cout << "Enter slot number: ";
        std::cin >> choice;

        // Clear the newline character from the buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try 
        {
            unsigned int slotNumber = std::stoi(choice);
            if (slotNumber >= 1 && slotNumber <= 6) 
            {
                auto module = m_cfgWrapper->getModuleBySlot(slotNumber);
                module->showModuleOnConsole();
                std::cout << "Press Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                continue; 
            } 
            else 
            {
                std::cout << "Invalid slot number. Must be between 1 and 6." << std::endl;
                std::cout << "Press Enter to select a new one or x to return to main menu...";
                std::cin.ignore();
                char decision = std::cin.get();
                if (decision == '\n') 
                {
                    continue; // Continue the loop to ask for input again
                }
                else if (decision == 'x' || decision == 'X') 
                {
                    // If signal is assigned then trigger it
                    if (showMainMenuSignal)
                    {
                        showMainMenuSignal();
                    }
                    break; // Exit the loop
                }
            }
        } 
        catch (const std::invalid_argument& e) 
        {
                std::cout << "Invalid slot number. Must be between 1 and 6." << std::endl;
                std::cout << "Press Enter to select a new one or x to return to main menu...";
                std::cin.ignore();
                char decision = std::cin.get();
                if (decision == '\n') 
                {
                    // Clear the error flags
                    std::cin.clear();
                    // Ignore the bad input
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue; // Continue the loop to ask for input again
                }
                else if (decision == 'x' || decision == 'X') 
                {
                    // If signal is assigned then trigger it
                    if (showMainMenuSignal)
                    {
                        // Clear the error flags
                        std::cin.clear();
                        //emit signal
                        showMainMenuSignal();
                    }
                    break; // Exit the loop
                } 
            
            
            
            
            std::cout << "Invalid input. Please enter a number between 1 and 6." << std::endl;

        }
    }
}*/

void moduleBySlotMenu::displayMenu() 
{
    std::string choice;

    while (true) 
    {
        clearConsole();
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓        Choose the slot     ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓  X -> return to main menu  ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl; 
        std::cout << "Enter slot number: ";
        std::cin >> choice;

        // Clear the buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == "x" || choice == "X") 
        {
            if (showMainMenuSignal)
            {
                showMainMenuSignal();
            }
            break;
        }

        try 
        {
            unsigned int slotNumber = std::stoi(choice);
            if (slotNumber >= 1 && slotNumber <= 6) 
            {
                auto module = m_cfgWrapper->getModuleBySlot(slotNumber);
                module->showModuleOnConsole();
                std::cout << "Press Enter to continue...";
                std::cin.get(); // Removed the extra ignore
                continue;
            } 
            else 
            {
                std::cout << "Invalid slot number. Must be between 1 and 6." << std::endl;
            }
        } 
        catch (const std::invalid_argument& e) 
        {
            std::cout << "Invalid input. Please enter a number between 1 and 6." << std::endl;
        }

        std::cout << "Press Enter to select a new one or x to return to main menu...";
        char decision = std::cin.get();
        if (decision == 'x' || decision == 'X') 
        {
            if (showMainMenuSignal)
            {
                showMainMenuSignal();
            }
            break;
        }
    }
}


     


void moduleBySlotMenu::clearConsole()
{
    // ANSI escape sequence to clear screen for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
}
