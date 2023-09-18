#ifndef moduleByAliasMenu_H
#define moduleByAliasMenu_H

#include <string>
#include <cstdlib>
#include "..\Ni wrappers\QNiSysConfigWrapper.h"
#include <functional>
#include <limits>

class moduleByAliasMenu {
public:
    moduleByAliasMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper);
    void displayMenu();
    void clearConsole();

private:
  std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;

public:
  std::function<void()>  showMainMenuSignal; 
};

#endif // moduleByAliasMenu_H
