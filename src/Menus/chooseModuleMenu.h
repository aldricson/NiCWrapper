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
    std::string displayChooseModuleMenu();
    void handleChoice(const std::string& choice);

    // Function to get the selected module
    NIDeviceModule* getSelectedModule() const;
    const char* getManuallySelectedModuleName() const;

    // Signals
    std::function<void()> backSignal            = nullptr;
    std::function<void()> succesSignal          = nullptr;
    std::function<void()> failedSignal          = nullptr;


private:
    // Private member variables and functions can be added here
    std::shared_ptr<QNiSysConfigWrapper> m_cfgWrapper;
    char m_manuallySelectedModuleName[256] = "";
    NIDeviceModule *m_manuallySelectedModule = nullptr;
};

#endif // CHOOSEMODULEMENU_H
