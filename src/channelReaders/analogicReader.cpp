#include "AnalogicReader.h"

void AnalogicReader::onOneShotValueReaded(double aValue)
{
    std::vector<std::string>           options;
    std::vector<std::function<void()>> actions;
    std::function<void()>              retryFunction;
    options.push_back(" 0 . Read again"             );    actions.push_back( [this](){this->manualReadOneShot();} );
    options.push_back(" 1 . Choose another channel" );    actions.push_back( [this](){this->displayChooseChannelMenu();});  
    options.push_back(" 2 . Choose another module"  );    actions.push_back( [this](){this->displayChooseChannelMenu();});
    options.push_back(" 3 . Main Menu"              );    actions.push_back( [this](){  if (this->showMainMenuSignal) {this->showMainMenuSignal();}});
    retryFunction =  [this,aValue](){this->onOneShotValueReaded(aValue);};
    clearConsole();
    displayMenu(std::to_string(aValue), options, actions, retryFunction); 
 
}
    



AnalogicReader::AnalogicReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance,
                               std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
{
    m_sysConfig = aSysConfigInstance;
    m_daqMx     = aDaqMxInstance;
}

AnalogicReader::~AnalogicReader()
{
    // Cleanup code here
}


void AnalogicReader::displayChooseModuleMenu()
{
    std::string choice;
    clearConsole();
    // Prepare the list of modules
    std::vector<std::string> moduleNamesVector;
    for (unsigned int i = 0; i < m_sysConfig->getModuleList().size(); ++i) 
    {
        std::string str = " "+std::to_string(i) + " . " + m_sysConfig->getModuleList()[i]->getAlias();
        moduleNamesVector.push_back(str);
    }
    moduleNamesVector.push_back(" x . Main Menu");
    displayMenu("Choose the module", moduleNamesVector);

    //Show what is selected
    if (std::strcmp(m_manuallySelectedModuleName, "") != 0)
    {
     std::cout << "Selected: " << m_manuallySelectedModuleName;
    }
    
    if (std::strcmp(m_manuallySelectedChanName, "") != 0)
    {
       std::cout << m_manuallySelectedChanName;
    }

    std::cout << "Enter your choice: ";
    std::cin >> choice; 
    std::cin.clear();
    std::cin.ignore();

    if (choice == "x" || choice == "X")
    {
        if (showMainMenuSignal)
        {
            showMainMenuSignal();
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
            if (m_sysConfig->getModuleList().size()==0)
            {
                std::cout  << "No module found" << std::endl;
                std::cout << "press Enter to return to main menu" << std::endl;
                std::cin.get();
                std::cin.clear();
                std::cin.ignore();
                if (showMainMenuSignal)
                {
                    showMainMenuSignal();
                }
            }

            if (selectedModule < m_sysConfig->getModuleList().size())   
            {
                 m_manuallySelectedModule      = m_sysConfig->getModuleList()[selectedModule];
                 std::strncpy(m_manuallySelectedModuleName, m_manuallySelectedModule->getAlias().c_str(), sizeof(m_manuallySelectedModuleName) - 1);
                 m_manuallySelectedModuleName[sizeof(m_manuallySelectedModuleName) - 1] = '\0';  // Null-terminate just in case
                 displayChooseChannelMenu();
            }
            else 
            {
                std::cout << "Invalid module selection. Press enter to choose one" << std::endl;
                std::cin.get();
                std::cin.clear();
                std::cin.ignore();
                if (showMainMenuSignal)
                {
                    showMainMenuSignal();
                }
            }
        }           
        else 
        {
            std::cout << "Invalid input. Press enter to choose one" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu();
        }
    }
}



void AnalogicReader::displayChooseChannelMenu()
{
    if (!m_manuallySelectedModule)
    {
            std::cout << "Error in :displayChooseChannelMenu(), m_manuallySelectedModule is nullptr\n Press enter to try again." << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu();
            return;
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
       std::cout << m_manuallySelectedChanName;
    }

    std::cout << std::endl;
    std::cout << "Enter your choice: ";
    std::cin >> choice; 
    std::cin.clear();
    std::cin.ignore();

    if (choice == "x" || choice == "X")
    {
        displayChooseModuleMenu();
    }

    else
    {
         unsigned int selectedChannel;
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
                if (showMainMenuSignal)
                {
                    showMainMenuSignal();
                }
                return;
            }
            if (selectedChannel < m_sysConfig->getModuleList().size())   
            {
                 std::strncpy(m_manuallySelectedChanName, 
                              m_manuallySelectedModule->getChanNames()[selectedChannel].c_str(),
                             sizeof(m_manuallySelectedChanName) - 1);
                 m_manuallySelectedChanName[sizeof(m_manuallySelectedChanName) - 1] = '\0';  // Null-terminate just in case
                 m_manuallySelectedChanIndex = selectedChannel;
                 displayShowValueMenu();
            }
            else 
            {
                std::cout << "Invalid channel selection. Press enter to choose one" << std::endl;
                std::cin.get();
                std::cin.clear();
                std::cin.ignore();
                displayChooseChannelMenu();
            }
        }
        else 
        {
            std::cout << "Invalid input. Press enter to choose one" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseChannelMenu();
        }
    }

}

void AnalogicReader::displayShowValueMenu()
{
    if (!m_manuallySelectedModule)
    {
            std::cout << "Error in :displayShowValueMenu(), m_manuallySelectedModule is nullptr\n Press enter to try again." << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu();
            return;
    }        
    std::string choice;
    std::vector<std::string> options;
    options.push_back(" 0 .  Read one shot");
    options.push_back(" 1 .  Read loop");
    options.push_back(" x .  Previous Menu");
    clearConsole();
    displayMenu("Read Value", options); 
     

    std::cout << std::endl;
    std::cout << "Enter your choice: ";
    std::cin >> choice; 
    std::cin.clear();
    std::cin.ignore();

    if (choice == "x" || choice == "X")
    {
        displayChooseModuleMenu();
    }
    else
    {
        unsigned int selectedAction;
        std::stringstream ss(choice);
        if (ss >> selectedAction && ss.eof()) 
        {
            switch (selectedAction)
            {
                case 0:
                {
                    manualReadOneShot();
                    break;
                }
            }
        }
    }
    
}

void AnalogicReader::clearConsole()
{
    // ANSI escape sequence to clear screen for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
}

void AnalogicReader::manualReadOneShot()
{
    if (!m_manuallySelectedModule)
    {
        std::cout << "Error in :manualReadOneShot(), m_manuallySelectedModule is nullptr\n Press enter to try again." << std::endl;
        std::cin.get();
        std::cin.clear();
        std::cin.ignore();
        displayChooseModuleMenu();
        return;   
    }
    

    moduleType modType = m_manuallySelectedModule->getModuleType();

    switch (modType)
    {
        case isAnalogicInputCurrent:
        {
            if (!m_daqMx)
            {
                std::cout << "Error in :manualReadOneShot(), m_daqMxis nullptr\n Press enter to try again." << std::endl;
                std::cin.get();
                std::cin.clear();
                std::cin.ignore();
                displayChooseModuleMenu();  
            }
            
            double value = m_daqMx->readCurrent(m_manuallySelectedModule,m_manuallySelectedChanIndex);
            onOneShotValueReaded(value);
            break;
        }

        case isAnalogicInputVoltage:
        {
            std::cout << "isAnalogicInputVoltage not implemented yet" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu(); 
            break;
        }

        case isDigitalInputVoltage:
        {
            std::cout << "isDigitalInputVoltage not implemented yet (may be to remove, even)" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu(); 
            break;
        }

        case isDigitalIOAndCounter:
        {
            std::cout << "isDigitalIOAndCounter not implemented yet" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu(); 
            break;
        }

        case isDigitalIO:
        {
            std::cout << "isDigitalIO not implemented yet" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu(); 
            break;
        }

       case isCounter:
        {
            std::cout << "isCounter not implemented yet" << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu(); 
            break;
        }
    }
}

// Getters
std::shared_ptr<QNiSysConfigWrapper> AnalogicReader::getSysConfig() const {
    return m_sysConfig;
}

std::shared_ptr<QNiDaqWrapper> AnalogicReader::getDaqMx() const {
    return m_daqMx;
}

// Setters
void AnalogicReader::setSysConfig(const std::shared_ptr<QNiSysConfigWrapper>& newSysConfig) {
    m_sysConfig = newSysConfig;
    if (sysConfigChangedSignal) {
        sysConfigChangedSignal(m_sysConfig, this);
    }
}

void AnalogicReader::setDaqMx(const std::shared_ptr<QNiDaqWrapper>& newDaqMx) {
    m_daqMx = newDaqMx;
    if (daqMxChangedSignal) {
        daqMxChangedSignal(m_daqMx, this);
    }
}
