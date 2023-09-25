#include "moduleByAliasMenu.h"
#include <iostream>

moduleByAliasMenu::moduleByAliasMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper) {
    m_cfgWrapper = aConfigWrapper;
}

void moduleByAliasMenu::displayAliasMenu() 
{
    std::string title = "Choose the alias";
    std::vector<std::string> options;
    options.push_back(" type the Alias of the module (e.g. : Mod1)");
    options.push_back(" 0 . Main Menu");
    clearConsole();
    displayMenu(title,options);
    std::string alias;
    std::cout << "Enter alias: ";
    std::cin >> alias;
    std::cin.clear();
    std::cin.ignore();
    if (alias == "0") 
    {
            if (showMainMenuSignal)
            {
                showMainMenuSignal();
            }
    }
    else
    {
          try 
        {
            auto module = m_cfgWrapper->getModuleByAlias(alias);
            if (module != nullptr) 
            {
                module->showModuleOnConsole();
                std::cout << "Press Enter to continue...";
                std::cin.get();
                displayAliasMenu();
            } 
        } 
        catch (const std::invalid_argument& e) 
        {
            std::cout << "Invalid alias. No such module exists." << std::endl;
            std::cout << "Press Enter to continue...";
            std::cin.get();
            displayAliasMenu();
        }

    }
}


