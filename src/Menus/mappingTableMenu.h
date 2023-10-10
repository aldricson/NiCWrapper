#ifndef MAPPING_TABLE_MENU_H
#define MAPPING_TABLE_MENU_H

#include <memory>
#include <functional>
#include "../stringUtils/stringUtils.h"
#include "../NiModulesDefinitions/NIDeviceModule.h"
#include "../NiWrappers/QNiSysConfigWrapper.h"


class StringGrid;
class ChooseModuleMenu;
class ChooseChannelMenu;

class MappingTableMenu {
public:
    MappingTableMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper);
    void displayTableMenu();

private:
    std::shared_ptr<QNiSysConfigWrapper> m_sysConfig    ;
    std::shared_ptr<StringGrid>          m_mappingTable ;
    std::shared_ptr<ChooseModuleMenu>    m_moduleMenu   ;
    std::shared_ptr<ChooseChannelMenu>   m_channelMenu  ;
    NIDeviceModule *m_selectedModule = nullptr;
    char m_moduleName[256];
    char m_chanName  [256];
    void addMapping();
    void clearSelectedModule();
    
    //slots
    void onChooseMenuFailed();
    void onChooseMenuSucces();

};

#endif