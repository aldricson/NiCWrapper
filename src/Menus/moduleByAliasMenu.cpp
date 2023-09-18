#include "moduleByAliasMenu.h"
#include <iostream>

moduleByAliasMenu::moduleByAliasMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper) {
    m_cfgWrapper = aConfigWrapper;
}

void moduleByAliasMenu::displayMenu() 
{
    std::string alias;

    while (true) 
    {
        clearConsole();
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓        Choose the alias    ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓  X -> return to main menu  ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
        std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl; 
        std::cout << "Enter alias: ";
        std::cin >> alias;

        // Clear the buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (alias == "x" || alias == "X") 
        {
            if (showMainMenuSignal)
            {
                showMainMenuSignal();
            }
            break;
        }

        try 
        {
            auto module = m_cfgWrapper->getModuleByAlias(alias);
            if (module != nullptr) 
            {
                module->showModuleOnConsole();
                std::cout << "Press Enter to continue...";
                std::cin.get();
                continue;
            } 
        } 
        catch (const std::invalid_argument& e) 
        {
            std::cout << "Invalid alias. No such module exists." << std::endl;
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



void moduleByAliasMenu::clearConsole()
{
    // ANSI escape sequence to clear screen for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
}
