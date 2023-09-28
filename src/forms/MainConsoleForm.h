#ifndef MAIN_CONSOLE_FORM_H
#define MAIN_CONSOLE_FORM_H

#include "../FTXUI/component/captured_mouse.hpp"      // for ftxui
#include "../FTXUI/component/component.hpp"           // for Menu
#include "../FTXUI/component/component_options.hpp"   // for MenuOption
#include "../FTXUI/component/screen_interactive.hpp"  // for ScreenInteractive
#include <iostream>
#include <functional>
#include <memory>

using namespace ftxui;

class MainConsoleForm {
public:
    MainConsoleForm();
    void Show();

    // Signals
    std::function<void()>    ReadValueOneShotSignal;
    std::function<void()>    ReadValuePollingSignal;
    std::function<void()>    WriteOutputSignal;
    std::function<void(int)> SelectModuleChangedSignal;
    std::function<void(int)> SelectChannelChangedSignal;

private:
    ftxui::Component m_screen;
    ftxui::Component m_layer;
    //buttons
    ftxui::Component m_readValueOneShotButton;
    ftxui::Component m_readValuePollingButton;
    ftxui::Component m_writeOutputButton;
    ftxui::Component m_ExitButton;
    ftxui::Component m_ModuleMenu;
    ftxui::Component m_ChannelMenu;
    int m_moduleMenuIndex  = 0;
    int m_channelMenuIndex = 0;


    int selectedModuleIndex_ = 0;  // Changed from std::wstring to int
    int selectedChannelIndex_ = 0; // Changed from std::wstring to int
    
    void initButtons();
    void initmoduleMenuOptions (std::vector<std::string> moduleAlaises);
    void initChannelMenuOptions(std::vector<std::string> channelNames);
    void initLayer();

    //private slots
    void OnReadValueOneShotClicked();
    void OnReadValuePollingClicked();
    void onWriteOutputClicked();
    void onExitClicked();

    void onModuleMenuChanged();
    void onChannelMenuChanged();


    ftxui::MenuOption m_moduleMenuOptions  = ftxui::MenuOption::Vertical();
    ftxui::MenuOption m_channelMenuOptions = ftxui::MenuOption::Vertical();  
};

#endif // MAIN_CONSOLE_FORM_H
