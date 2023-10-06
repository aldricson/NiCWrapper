#include "mappingTableMenu.h"
#include "../stringUtils/stringGrid.h"
//NOT READY YET

MappingTableMenu::MappingTableMenu()
{
    m_mappingTable = std::make_shared<StringGrid>();
    m_mappingTable->loadCsv("./mapping.csv");
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
}
