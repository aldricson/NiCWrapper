#include "mainMenu.h"
#include <iostream>
#include "moduleBySlotMenu.h"
#include "moduleByAliasMenu.h"
#include "mappingTableMenu.h"


mainMenu::mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper,
                   std::shared_ptr<AnalogicReader>      anAnalogicReader,
                   std::shared_ptr<DigitalReader>       aDigitalReader,
                   std::shared_ptr<DigitalWriter>       aDigitalWriter,
                   std::shared_ptr<NItoModbusBridge>    aBridge)
{
    //the wrapper around NiDaqMx library
    m_cfgWrapper     = aConfigWrapper  ;
    //helper object to read analogic channels
    m_analogicReader = anAnalogicReader;
    //helper object to read digital channels
    m_digitalReader  = aDigitalReader  ;
    //helper object to wriite digoital channels (eg relays)
    m_digitalWriter  = aDigitalWriter;
    //bridge between crio and modbus
    m_bridge         = aBridge;     
    //create sub menus 
    m_moduleBySlotMenu  = std::make_shared<moduleBySlotMenu> (m_cfgWrapper);
    m_moduleByAliasMenu = std::make_shared<moduleByAliasMenu>(m_cfgWrapper);
    m_mappingTableMenu  = std::make_shared<MappingTableMenu> (m_cfgWrapper);
    //TODO add missing sub menus

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
    options.push_back(" 0 . show module by Alias       "); actions.push_back( [this](){this-> m_moduleByAliasMenu ->displayAliasMenu       ();});
    options.push_back(" 1 . show module by Slotl       "); actions.push_back( [this](){this-> m_moduleBySlotMenu  ->displaySlotMenu        ();});  
    options.push_back(" 2 . read analogic  value       "); actions.push_back( [this](){this-> m_analogicReader    ->displayChooseModuleMenu();});
    options.push_back(" 3 . read digital   value       "); actions.push_back( [this](){this-> m_digitalReader     ->displayChooseModuleMenu();});
    options.push_back(" 4 . test modbus server in simu "); actions.push_back( [this](){this-> m_bridge            ->startModbusSimulation  ();});
    options.push_back(" 5 . Show modbus mapping table  "); actions.push_back( [this](){this-> m_mappingTableMenu  ->displayTableMenu       ();});
    options.push_back(" 6 . Exit                       "); actions.push_back( [this](){   std::cout << "Exiting..." << std::endl;
                                                                                        if (this->exitProgramSignal)
                                                                                            {
                                                                                                this->exitProgramSignal();
                                                                                            }
                                                                                    });

    retryFunction =  [this]() {  this->displayMainMenu();};
    clearConsole();
    displayMenu("MAIN MENU", options, actions, retryFunction); 
    
    
}



void mainMenu::onDisplayMainMenu()
{
    displayMainMenu();
}
