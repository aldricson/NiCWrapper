#include "AnalogicReader.h"


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
    std::vector<std::string> moduleNamesVector;
    unsigned int nbChars = 0;
    for (unsigned int i = 0; i < m_sysConfig->getModuleList().size(); ++i)
    {
        std::string str = std::to_string(i) + " . " + m_sysConfig->getModuleList()[i]->getAlias();
        moduleNamesVector.push_back(str);
        if (str.length() > nbChars) nbChars = str.length();
    }
    std::string title = "Choose the module";
    if (title.length() > nbChars) nbChars = title.length();
    nbChars += 2;
    std::string line = "";
    for (unsigned int i = 0; i < nbChars; ++i) line += "░";
    

    // Function to center-align a string within a given width
    auto centerAlignString = [nbChars](const std::string& str) {
        unsigned int totalSpaces = nbChars - str.length();
        unsigned int spacesBefore = totalSpaces / 2;
        unsigned int spacesAfter = totalSpaces - spacesBefore;
        std::string spacesBeforeStr(spacesBefore, ' ');
        std::string spacesAfterStr(spacesAfter, ' ');
        return "░" + spacesBeforeStr + str + spacesAfterStr + "░";
    };

    // Output the centered title
    std::cout << line.c_str()<<"░░"  << std::endl;
    std::cout << centerAlignString(title) << std::endl;
    std::cout << line.c_str()<<"░░"  << std::endl;

    // Output each centered module name
    for (const auto& moduleName : moduleNamesVector)
    {
        std::cout << centerAlignString(moduleName) << std::endl;
    }

    std::cout << line.c_str()<<"░░"  << std::endl;
    if (std::strcmp(m_manuallySelectedModuleName, "") != 0)
    {
     std::cout << "Selected: " << m_manuallySelectedModuleName << std::endl;
     displayChooseChannelMenu();
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
  
   //TODO

   /* std::string choice;
    clearConsole();
    std::vector<std::string> moduleNamesVector;
    unsigned int nbChars = 0;
    for (unsigned int i = 0; i < m_sysConfig->getModuleList().size(); ++i)
    {
        std::string str = std::to_string(i) + " . " + m_sysConfig->getModuleList()[i]->getAlias();
        moduleNamesVector.push_back(str);
        if (str.length() > nbChars) nbChars = str.length();
    }
    std::string title = "Choose the Channel";
    if (title.length() > nbChars) nbChars = title.length();
    nbChars += 2;
    std::string line = "";
    for (unsigned int i = 0; i < nbChars; ++i) line += "░";
    

    // Function to center-align a string within a given width
    auto centerAlignString = [nbChars](const std::string& str) {
        unsigned int totalSpaces = nbChars - str.length();
        unsigned int spacesBefore = totalSpaces / 2;
        unsigned int spacesAfter = totalSpaces - spacesBefore;
        std::string spacesBeforeStr(spacesBefore, ' ');
        std::string spacesAfterStr(spacesAfter, ' ');
        return "░" + spacesBeforeStr + str + spacesAfterStr + "░";
    };

    // Output the centered title
    std::cout << line.c_str()<<"░░"  << std::endl;
    std::cout << centerAlignString(title) << std::endl;
    std::cout << line.c_str()<<"░░"  << std::endl;
    */
   
}

void AnalogicReader::clearConsole()
{
    // ANSI escape sequence to clear screen for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
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
