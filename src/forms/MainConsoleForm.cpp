#include "MainConsoleForm.h"


MainConsoleForm::MainConsoleForm() 
{
    // Initialize components
    initButtons();

}

void MainConsoleForm::Show() 
{
 //  m_screen->Loop();
}

void MainConsoleForm::initLayer()
{
    Components line1 = {m_ModuleMenu,  m_readValueOneShotButton};
    Components line2 = {m_ChannelMenu, m_readValuePollingButton};
    Components line3 = {m_writeOutputButton};

    m_layer =  Container::Vertical({
               Container::Horizontal(line1),
               Container::Horizontal(line2),
               Container::Horizontal(line3)
               });
}

void MainConsoleForm::initButtons()
{
    m_readValueOneShotButton  =  Button("Read Value One Shot", [&] {OnReadValueOneShotClicked();});
    m_readValuePollingButton  =  Button("Read Value Polling" , [&] {OnReadValuePollingClicked();});
    m_writeOutputButton       =  Button("Write Output"       , [&] {onWriteOutputClicked     ();});
    m_ExitButton              =  Button("Exit"               , [&] {onExitClicked            ();});
}

void MainConsoleForm::initmoduleMenuOptions(std::vector<std::string> moduleAlaises)
{
    // Set the list of entries
     std::vector<std::string> tempEntries = std::move(moduleAlaises);
     m_moduleMenuOptions.entries = ConstStringListRef(&tempEntries);
    // Set the initially selected index (0-based)
    m_moduleMenuOptions.selected = 0;
    // Style: Underline the selected option
    m_moduleMenuOptions.underline.enabled = true;
    m_ModuleMenu = ftxui::Menu(m_moduleMenuOptions);
}

void MainConsoleForm::initChannelMenuOptions(std::vector<std::string> channelNames)
{
        // Set the list of entries
     std::vector<std::string> tempEntries = std::move(channelNames);
     m_channelMenuOptions.entries = ConstStringListRef(&tempEntries);
    // Set the initially selected index (0-based)
    m_channelMenuOptions.selected = 0;
    // Style: Underline the selected option
    m_channelMenuOptions.underline.enabled = true;
    m_ChannelMenu = ftxui::Menu(m_channelMenuOptions);
}

void MainConsoleForm::OnReadValueOneShotClicked()
{
    //TODO
}

void MainConsoleForm::OnReadValuePollingClicked()
{
    //TODO
}

void MainConsoleForm::onWriteOutputClicked()
{
    //TODO
}

void MainConsoleForm::onExitClicked()
{
    //TODO
}
