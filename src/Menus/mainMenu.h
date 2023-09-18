#ifndef MAINMENU_H
#define MAINMENU_H

#include <string>
#include <cstdlib>
#include "..\Ni wrappers\QNiSysConfigWrapper.h"
#include <functional>

//forward declarations
class moduleBySlotMenu;
class moduleByAliasMenu;

class mainMenu {
public:
    mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper);

    void displayMenu();
    void clearConsole();

private:
  std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;
  std::shared_ptr<moduleBySlotMenu>    m_moduleBySlotMenu;
  std::shared_ptr<moduleByAliasMenu>   m_moduleByAliasMenu; 
  //---------- slots ------------
  void onDisplayMainMenu();
};

#endif // MAINMENU_H
