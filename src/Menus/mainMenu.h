#ifndef MAINMENU_H
#define MAINMENU_H

#include <string>
#include <cstdlib>
#include "..\Ni wrappers\QNiSysConfigWrapper.h"
#include "..\channelReaders\analogicReader.h"
#include "..\stringUtils\stringUtils.h"
#include <functional>

//forward declarations
class moduleBySlotMenu;
class moduleByAliasMenu;


class mainMenu {
public:
    mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper,
             std::shared_ptr<AnalogicReader>      anAnalogicReader);

    void displayMainMenu();
    void clearConsole();

    //signals
    std::function<void()>  exitProgramSignal; 

private:
  std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;
  std::shared_ptr<moduleBySlotMenu>    m_moduleBySlotMenu;
  std::shared_ptr<moduleByAliasMenu>   m_moduleByAliasMenu; 
  std::shared_ptr<AnalogicReader>      m_analogicReader;
  //---------- slots ------------
  void onDisplayMainMenu();
};

#endif // MAINMENU_H
