#ifndef moduleBySlotMenu_H
#define moduleBySlotMenu_H

#include <string>
#include <cstdlib>
#include "..\Ni wrappers\QNiSysConfigWrapper.h"
#include <functional>
#include <limits>

class moduleBySlotMenu {
public:
    moduleBySlotMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper);
    void displayMenu();
    void clearConsole();

private:
  std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;

public:
  std::function<void()>  showMainMenuSignal; 
};

#endif // moduleBySlotMenu_H
