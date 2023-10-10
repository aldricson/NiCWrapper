#include "mappingTableMenu.h"
#include "../stringUtils/stringGrid.h"
#include "../Menus/chooseModuleMenu.h"
#include "../Menus/chooseChannelMenu.h"

//NOT READY YET

MappingTableMenu::MappingTableMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper)
{
    m_sysConfig    =  aConfigWrapper;
    m_mappingTable =  std::make_shared<StringGrid>();
    m_mappingTable -> loadCsv("./mapping.csv");
    m_moduleMenu   =  std::make_shared<ChooseModuleMenu>(m_sysConfig);
    m_moduleMenu->failedSignal  = std::bind(&MappingTableMenu::onChooseMenuFailed,this);
    m_moduleMenu->succesSignal  = std::bind(&MappingTableMenu::onChooseMenuSucces,this);

    m_channelMenu = std::make_shared<ChooseChannelMenu>(m_sysConfig);
}

void MappingTableMenu::displayTableMenu()
{
    //clear the console
    clearConsole();
    //render the string grid on screen
    m_mappingTable->renderGrid();
    //create the menu
    std::vector<std::string>           options;
    std::vector<std::function<void()>> actions;
    std::function<void()>              retryFunction;
    options.push_back(" 0 . Add new mapping  " ); actions.push_back( [this](){this->addMapping();});
    options.push_back(" 1 . Remove mapping   " ); actions.push_back( [this](){});
    options.push_back(" 2 . Move mapping up  " ); actions.push_back( [this](){});
    options.push_back(" 3 . Move mapping down" ); actions.push_back( [this](){});
    options.push_back(" 4 . Previous Menu    " ); actions.push_back( [this](){});

    retryFunction =  [this]() {  this->displayTableMenu();};
    displayMenu("CHANNEL TO MODBUS MAPPING", options, actions, retryFunction);
}

void MappingTableMenu::addMapping()
{
    clearSelectedModule();
    clearConsole();
    //render the string grid on screen
    m_mappingTable->renderGrid();
    std::string choice;
    choice = m_moduleMenu->displayChooseModuleMenu(showAll,false);
    if (choice == "")
    {
        std::cout << "Serious problem detected with module sub menu in addMapping()" << std::endl;
        std::cout << "Press enter to continue" << std::endl;
        std::cin.get();
        std::cin.clear();
        std::cin.ignore();
        clearConsole();
         //render the string grid on screen
        m_mappingTable->renderGrid();
        addMapping();
    }
    else
    m_moduleMenu->handleChoice(choice);
}

void MappingTableMenu::clearSelectedModule()
{
    if (m_selectedModule)
    {
        m_selectedModule = nullptr;
        m_moduleName[0]  = '\0';
        m_chanName  [0]  = '\0';
    }
}

void MappingTableMenu::onChooseMenuFailed()
{
    clearConsole();
    //render the string grid on screen
    m_mappingTable->renderGrid();
    addMapping();
}

void MappingTableMenu::onChooseMenuSucces()
{
    clearConsole();
    //render the string grid on screen
    m_mappingTable->renderGrid();
    //get the module
    m_selectedModule   = m_moduleMenu->getSelectedModule();
    //get the module name
    std::strncpy(m_moduleName, m_selectedModule->getAlias().c_str(), sizeof(m_moduleName) - 1);
                 m_moduleName[sizeof(m_moduleName) - 1] = '\0';  // Null-terminate just in case
    std::string choice = m_channelMenu->displayChooseChannelMenu(m_selectedModule);
     if (choice == "")
    {
        std::cout << "Serious problem detected with channel sub menu in onChooseMenuSucces()" << std::endl;
        std::cout << "Press enter to continue" << std::endl;
        std::cin.get();
        std::cin.clear();
        std::cin.ignore();
        clearConsole();
         //render the string grid on screen
        m_mappingTable->renderGrid();
        addMapping();
    }
    m_channelMenu->handleChoice(choice);
}
