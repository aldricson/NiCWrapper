#include "mainMenu.h"
#include <iostream>
#include "moduleBySlotMenu.h"
#include "moduleByAliasMenu.h"

mainMenu::mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper) {
    m_cfgWrapper = aConfigWrapper;
    m_moduleBySlotMenu  = std::make_shared<moduleBySlotMenu> (m_cfgWrapper);
    m_moduleByAliasMenu = std::make_shared<moduleByAliasMenu>(m_cfgWrapper);
    //connect "signals" to "slots"
    m_moduleBySlotMenu ->showMainMenuSignal = std::bind(&mainMenu::onDisplayMainMenu,this);
    m_moduleByAliasMenu->showMainMenuSignal = std::bind(&mainMenu::onDisplayMainMenu,this);
    clearConsole();
    displayMenu();
}


/*void mainMenu::displayMenu() 
{
    std::string choice;
    while (true) 
    {
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

        if (choice == "1") 
        {
            m_moduleByAliasMenu->displayMenu();
        } 
        else if (choice == "2") 
        {
            m_moduleBySlotMenu->displayMenu();

        }
        else if (choice == "x" || choice == "X") 
        {
            std::cout << "Exiting..." << std::endl;
            break;
        } 
        else 
        {
            std::cout << "Invalid choice. Try again." << std::endl;
        }
    }
}*/

void mainMenu::displayMenu() 
{
    std::string choice;
    while (true) 
    {
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

        

        if (choice == "1") 
        {
            m_moduleByAliasMenu->displayMenu();
        } 
        else if (choice == "2") 
        {
            m_moduleBySlotMenu->displayMenu();
        }
        else if (choice == "x" || choice == "X") 
        {
            std::cout << "Exiting..." << std::endl;
            break;
        } 
        else 
        {
            std::cout << "Invalid choice. Try again." << std::endl;
        }
    }
}



void mainMenu::clearConsole()
{
    // ANSI escape sequence to clear screen for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
}

void mainMenu::onDisplayMainMenu()
{
    // Clear the buffer
    std::cin.clear() ;
    displayMenu();
}
