#ifndef CHOOSEMODULEMENU_H
#define CHOOSEMODULEMENU_H

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include "../stringUtils/stringUtils.h"
#include "../NiWrappers/QNiSysConfigWrapper.h"

class ChooseModuleMenu {
public:
    // Constructor
    ChooseModuleMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper);
    // Destructor
    ~ChooseModuleMenu();
    // Function to display the menu
    void displayChooseModuleMenu(bool errorToMainMenu);
    // Function to select a module
    int selectModule();


    //signals
    std::function<void()> showMainMenuSignal = nullptr;

private:
    // Private member variables and functions can be added here
    std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;
    char            m_manuallySelectedModuleName[256] = "";
    NIDeviceModule *m_manuallySelectedModule          = nullptr;
};

#endif // CHOOSEMODULEMENU_H
