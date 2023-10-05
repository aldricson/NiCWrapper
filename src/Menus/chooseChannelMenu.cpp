#include "chooseChannelMenu.h"

ChooseChannelMenu::ChooseChannelMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper)
{
    m_cfgWrapper = aConfigWrapper;
}

ChooseChannelMenu::~ChooseChannelMenu()
{
}

std::string ChooseChannelMenu::displayChooseChannelMenu(NIDeviceModule* aModule)
{
    m_manuallySelectedModule = aModule; 
    if (!m_manuallySelectedModule)
    {
            std::cout << "Error in :displayChooseChannelMenu(), m_manuallySelectedModule is nullptr\n Press enter to try again." << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            if (backSignal)
            {
              backSignal();
              return "";
            }
    }        
    std::string choice;
    clearConsole();
    // Prepare the list of channel names
    std::vector<std::string> channelNamesVector;
    for (unsigned int i = 0; i < m_manuallySelectedModule->getChanNames().size(); ++i) 
    {
        std::string str = " "+std::to_string(i) + " . " + m_manuallySelectedModule->getChanNames()[i];
        channelNamesVector.push_back(str);  
    }
    channelNamesVector.push_back(" x . Previous Menu");
    displayMenu("Choose the Channel", channelNamesVector);


    //Show what is selected
    if (std::strcmp(m_manuallySelectedModuleName, "") != 0)
    {
     std::cout << "Selected: " << m_manuallySelectedModuleName;
    }
    
    if (std::strcmp(m_manuallySelectedChanName, "") != 0)
    {
       std::cout <<m_manuallySelectedChanName<<std::endl;
    }
    std::cout << "Enter your choice: ";
    std::cin >> choice; 
    std::cin.clear();
    std::cin.ignore();
    return choice;

}

void ChooseChannelMenu::handleChoice(const std::string &choice)
{

    if (choice == "x" || choice == "X")
    {
        if (backSignal)
        {
            backSignal();
        }
    }
    else
    {
        
        std::stringstream ss(choice);
        if (ss >> selectedChannel && ss.eof()) 
        {
            // 'selectedChannel' now contains the unsigned int value entered by the user
                        //last security checks
            if ( m_manuallySelectedModule->getChanNames().size()==0)
            {
                std::cout  << "No channel found" << std::endl;
                std::cout << "press Enter to return to main menu" << std::endl;
                std::cin.get();
                std::cin.clear();
                std::cin.ignore();
                if (failedSignal)
                {
                    failedSignal();
                }
                return;
            }
            if (selectedChannel < m_manuallySelectedModule->getChanNames().size())   
            {
                 std::strncpy(m_manuallySelectedChanName, 
                              m_manuallySelectedModule->getChanNames()[selectedChannel].c_str(),
                             sizeof(m_manuallySelectedChanName) - 1);
                 m_manuallySelectedChanName[sizeof(m_manuallySelectedChanName) - 1] = '\0';  // Null-terminate just in case
                 m_manuallySelectedChanIndex = selectedChannel;
                 if (succesSignal)
                 {
                    succesSignal();
                 }
            }
            else 
            {
                std::cout << "Invalid channel selection. Press enter to choose one" << std::endl;
                std::cin.get();
                std::cin.clear();
                std::cin.ignore();
                if (backSignal)
                {
                    backSignal();
                }
            }
        }
        else 
        {
            std::cout << "Invalid input. Press enter to choose one" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            if (backSignal)
            {
               backSignal();
            }
        }
    }
}
