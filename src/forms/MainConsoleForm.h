#ifndef MAIN_CONSOLE_FORM_H
#define MAIN_CONSOLE_FORM_H

/*#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Menu
#include "ftxui/component/component_options.hpp"   // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include <iostream>
#include <functional>
#include <memory>

using namespace ftxui;

class MainConsoleForm {
public:
    MainConsoleForm();
    void Show();

    // Signals
    std::function<void()> onReadValueOneShot;
    std::function<void()> onReadValuePolling;
    std::function<void()> onWriteOutput;
    std::function<void(int)> onSelectModuleChanged;
    std::function<void(int)> onSelectChannelChanged;

private:
    ftxui::Component screen_;
    ftxui::Component container_;

    ftxui::Menu test;

    int selectedModuleIndex_ = 0;  // Changed from std::wstring to int
    int selectedChannelIndex_ = 0; // Changed from std::wstring to int
    void initmoduleMenuOptions(std::vector<std::string> moduleAlaises);


    ftxui::MenuOption m_moduleMenuOptions = ftxui::MenuOption::Vertical(); 
};*/

#endif // MAIN_CONSOLE_FORM_H
