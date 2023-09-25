#include "moduleBySlotMenu.h"
#include <iostream>

moduleBySlotMenu::moduleBySlotMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper) {
    m_cfgWrapper = aConfigWrapper;

}


void moduleBySlotMenu::displaySlotMenu()
{
    
    std::string title = "Choose the slot number";
    std::vector<std::string> options;
    options.push_back(" choose the slot number (from 1)");
    options.push_back(" 0 . Main Menu");
    clearConsole();
    displayMenu(title,options);
    std::string choice;
    std::cout << "Enter your choice: ";
    std::cin >> choice;
    std::cin.clear();
    std::cin.ignore();
    unsigned int slotNumber;
    std::stringstream ss(choice);
    if (ss >> slotNumber && ss.eof())
    {
        try 
        {
            if (slotNumber >= 1) 
            {
                auto module = m_cfgWrapper->getModuleBySlot(slotNumber);
                module->showModuleOnConsole();
                std::cout << "Press Enter to continue...";
                std::cin.get(); // Removed the extra ignore
                displaySlotMenu();
            }
            else if (slotNumber == 0)
            {
                if (showMainMenuSignal)
                {
                    showMainMenuSignal();
                }
            } 
        } 
        catch (const std::invalid_argument& e) 
        {
            std::cout << "Invalid input. This slot may not exists" << std::endl;
            std::cout << "Press Enter to retry...";
            std::cin.get(); // Removed the extra ignore
            displaySlotMenu();
        }
           
    } 
    else 
    {
            //Not a number
            std::cout << "Invalid selection." << std::endl;
            std::cout << "Press Enter to retry...";
            std::cin.get(); // Removed the extra ignore
            displaySlotMenu();        
    }
  
}
