#ifndef MAINMENU_H
#define MAINMENU_H

#include <string>
#include <cstdlib>
#include "..\Ni wrappers\QNiSysConfigWrapper.h"

class mainMenu {
public:
    mainMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper);
    void displayMenu();
    void clearConsole();

private:
  std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper; 
};

#endif // MAINMENU_H
