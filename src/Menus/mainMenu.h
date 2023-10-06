#ifndef MAINMENU_H
#define MAINMENU_H

#include <string>
#include <cstdlib>
#include "../NiWrappers/QNiSysConfigWrapper.h"
#include "../channelReaders/analogicReader.h"
#include "../channelReaders/digitalReader.h"
#include "../stringUtils/stringUtils.h"
#include <functional>

//forward declarations
class moduleBySlotMenu;
class moduleByAliasMenu;


class mainMenu {
public:
    mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper,
             std::shared_ptr<AnalogicReader>      anAnalogicReader,
             std::shared_ptr<DigitalReader>       aDigitalReader);

    void displayMainMenu();


    //signals
    std::function<void()>  exitProgramSignal; 

private:
  std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;
  std::shared_ptr<moduleBySlotMenu>    m_moduleBySlotMenu;
  std::shared_ptr<moduleByAliasMenu>   m_moduleByAliasMenu; 
  std::shared_ptr<AnalogicReader>      m_analogicReader;
  std::shared_ptr<DigitalReader>       m_digitalReader;
  //---------- slots ------------
  void onDisplayMainMenu();
};

#endif // MAINMENU_H
