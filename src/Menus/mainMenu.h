#ifndef MAINMENU_H
#define MAINMENU_H

#include <string>
#include <cstdlib>
#include "../NiWrappers/QNiSysConfigWrapper.h"
#include "../channelReaders/analogicReader.h"
#include "../channelReaders/digitalReader.h"
#include "../channelWriters/digitalWriter.h"
#include "../Bridge/niToModbusBridge.h"
#include "../stringUtils/stringUtils.h"
#include <functional>

//forward declarations
class moduleBySlotMenu;
class moduleByAliasMenu;
class MappingTableMenu;


class mainMenu {
public:
    mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper,
             std::shared_ptr<AnalogicReader>      anAnalogicReader,
             std::shared_ptr<DigitalReader>       aDigitalReader,
             std::shared_ptr<DigitalWriter>       aDigitalWriter,
             std::shared_ptr<NItoModbusBridge>    aBridge
             );

    void displayMainMenu();


    //signals
    std::function<void()>  exitProgramSignal; 

private:
  std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;
  std::shared_ptr<moduleBySlotMenu>    m_moduleBySlotMenu;
  std::shared_ptr<moduleByAliasMenu>   m_moduleByAliasMenu;
  std::shared_ptr<MappingTableMenu>    m_mappingTableMenu; 
  std::shared_ptr<AnalogicReader>      m_analogicReader;
  std::shared_ptr<DigitalReader>       m_digitalReader;
  std::shared_ptr<DigitalWriter>       m_digitalWriter;
  std::shared_ptr<NItoModbusBridge>    m_bridge;
  //---------- slots ------------
  void onDisplayMainMenu();
};

#endif // MAINMENU_H
