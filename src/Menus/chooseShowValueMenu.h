#ifndef ChooseShowValueMenu_H
#define ChooseShowValueMenu_H

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include "../stringUtils/stringUtils.h"
#include "../NiWrappers/QNiSysConfigWrapper.h"

class ChooseShowValueMenu {
public:
    // Constructor
    ChooseShowValueMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper);
    // Destructor
    ~ChooseShowValueMenu();
    // Function to display the menu
    std::string displayChooseShowValueMenu();
   // void handleChoice(const std::string& choice);
    unsigned int selectedChannel;  
    // Signals
    std::function<void()> backSignal            = nullptr;
    std::function<void()> succesSignal          = nullptr;
    std::function<void()> failedSignal          = nullptr;

private:
    // Private member variables and functions can be added here
    std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;
    char m_manuallySelectedModuleName[256] = "";
    char m_manuallySelectedChanName[256] = "";
    unsigned int m_manuallySelectedChanIndex       = 0;
    NIDeviceModule *m_manuallySelectedModule = nullptr;};

#endif // ChooseShowValueMenu_H
