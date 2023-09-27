#ifndef moduleBySlotMenu_H
#define moduleBySlotMenu_H

#include <string>
#include <cstdlib>
#include "../NiWrappers/QNiSysConfigWrapper.h"
#include "../stringUtils/stringUtils.h"
#include <functional>
#include <limits>
#include <vector>

class moduleBySlotMenu {
public:
    moduleBySlotMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper);
    void displaySlotMenu();

private:
  std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;

public:
  std::function<void()>  showMainMenuSignal; 
};

#endif // moduleBySlotMenu_H
