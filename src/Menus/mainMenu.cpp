#include "mainMenu.h"
#include <iostream>
#include "moduleBySlotMenu.h"
#include "moduleByAliasMenu.h"


mainMenu::mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper,
                  std::shared_ptr<AnalogicReader>      anAnalogicReader)
{
    m_cfgWrapper     = aConfigWrapper;
    m_analogicReader = anAnalogicReader;
    m_moduleBySlotMenu  = std::make_shared<moduleBySlotMenu> (m_cfgWrapper);
    m_moduleByAliasMenu = std::make_shared<moduleByAliasMenu>(m_cfgWrapper);
    //connect "signals" to "slots"
    m_moduleBySlotMenu ->showMainMenuSignal = std::bind(&mainMenu::onDisplayMainMenu,this);
    m_moduleByAliasMenu->showMainMenuSignal = std::bind(&mainMenu::onDisplayMainMenu,this);
    m_analogicReader   ->showMainMenuSignal = std::bind(&mainMenu::onDisplayMainMenu,this);
    clearConsole();
    displayMainMenu();
}


void mainMenu::displayMainMenu() 
{
    std::vector<std::string>           options;
    std::vector<std::function<void()>> actions;
    std::function<void()>              retryFunction;
    options.push_back(" 0 . show module by Alias" );    actions.push_back( [this](){this->m_moduleByAliasMenu->displayMenu();} );
    options.push_back(" 1 . show module by Slotl" );    actions.push_back( [this](){this->m_moduleBySlotMenu->displayMenu();});  
    options.push_back(" 2 . read a channel value" );    actions.push_back( [this](){this-> m_analogicReader->displayChooseModuleMenu();});
    options.push_back(" 3 . Exit"                 );    actions.push_back( [this](){   std::cout << "Exiting..." << std::endl;
                                                                                        if (this->exitProgramSignal)
                                                                                            {
                                                                                                this->exitProgramSignal();
                                                                                            }
                                                                                    });

    retryFunction =  [this]() {  this->displayMainMenu();};
    clearConsole();
    displayMenu("MAIN MENU", options, actions, retryFunction); 
    
    
    
  /*  std::string choice;
    clearConsole();
    std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
    std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓          MAIN MENU         ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
    std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓   0. show module by Alias  ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓   1. show module by Slot   ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓   2. read a channel value  ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓   X.       exit            ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓                            ▓▒░✩" << std::endl;
    std::cout << "✩░▒▓▒░░░▒░░░▒░░░▒░░░▒░░░▒░░░░░░▒▓▒░✩" << std::endl;
    std::cout << "Enter your choice: ";
    choice = std::cin.get();    
    std::cin.clear();
    std::cin.ignore();
    if (choice == "0") 
    {
        m_moduleByAliasMenu->displayMenu();
    } 
    else if (choice == "1") 
    {
         m_moduleBySlotMenu->displayMenu();
    }
    else if (choice == "2") 
    {
         m_analogicReader->displayChooseModuleMenu();
    }
    else if (choice == "x" || choice == "X") 
    {
        
        std::cout << "Exiting..." << std::endl;
        if (exitProgramSignal)
        {
                exitProgramSignal();
        }
    } 
    else 
    {
            std::cout << "Invalid choice. Try again." << std::endl;
            displayMenu();
    }*/
    
}



void mainMenu::clearConsole()
{
    // ANSI escape sequence to clear screen for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
}

void mainMenu::onDisplayMainMenu()
{
    displayMainMenu();
}
