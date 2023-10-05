#include "chooseModuleMenu.h"
#include <iostream>
#include <sstream>
#include <cstring>

ChooseModuleMenu::ChooseModuleMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper)
{
    m_cfgWrapper = aConfigWrapper;
}

ChooseModuleMenu::~ChooseModuleMenu()
{
}

NIDeviceModule* ChooseModuleMenu::getSelectedModule() const
{
    return m_manuallySelectedModule;
}

const char *ChooseModuleMenu::getManuallySelectedModuleName() const
{
    return m_manuallySelectedModuleName;
}

std::string ChooseModuleMenu::displayChooseModuleMenu()
{
    std::string choice;
    clearConsole();

    // Prepare the list of modules
    std::vector<std::string> moduleNamesVector;
    for (unsigned int i = 0; i < m_cfgWrapper->getModuleList().size(); ++i) 
    {
        std::string str = " " + std::to_string(i) + " . " + m_cfgWrapper->getModuleList()[i]->getAlias();
        moduleNamesVector.push_back(str);
    }
    moduleNamesVector.push_back(" x . Main Menu");
    displayMenu("Choose the module", moduleNamesVector);

    // Show what is selected
    if (m_manuallySelectedModule)
    {
        std::cout << "Selected: " << m_manuallySelectedModuleName << std::endl;
    }
    
    std::cout << "Enter your choice: ";
    std::cin >> choice; 
    std::cin.clear();
    std::cin.ignore();
    return choice;
}


void ChooseModuleMenu::handleChoice(const std::string& choice)
{
    m_manuallySelectedModule     = nullptr;
    //this commented code is for reference until it works
   if (choice == "x" || choice == "X")
    {
        if (backSignal)
        {
            backSignal();
        }
    }
    else
    {
        
        unsigned int selectedModule;
        std::stringstream ss(choice);
        if (ss >> selectedModule && ss.eof()) 
        {
            
            // 'selectedModule' now contains the unsigned int value entered by the user
            //last security checks
            if (m_cfgWrapper->getModuleList().size()==0)
            {
                std::cout  << "No module found" << std::endl;
                std::cout << "press Enter to return" << std::endl;
                std::cin.get();
                std::cin.clear();
                std::cin.ignore();
                if (failedSignal)
                {
                    failedSignal();
                }
            }
            
            if (selectedModule < m_cfgWrapper->getModuleList().size())   
            {

                 m_manuallySelectedModule      = m_cfgWrapper->getModuleList()[selectedModule];
                 std::strncpy(m_manuallySelectedModuleName, m_manuallySelectedModule->getAlias().c_str(), sizeof(m_manuallySelectedModuleName) - 1);
                 m_manuallySelectedModuleName[sizeof(m_manuallySelectedModuleName) - 1] = '\0';  // Null-terminate just in case
                 if(succesSignal)
                 {
                    succesSignal();
                 }
            }
            else 
            {
                std::cout << "Invalid module selection. Press enter to choose one" << std::endl;
                std::cin.get();
                std::cin.clear();
                std::cin.ignore();
                if (failedSignal)
                {
                    failedSignal();
                }
            }
        }           
        else 
        {
            std::cout << "Invalid input. Press enter to choose one" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            if (failedSignal)
                {
                    failedSignal();
                }
        }
    }
}
