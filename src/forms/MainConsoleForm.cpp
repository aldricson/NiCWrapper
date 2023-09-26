#include "MainConsoleForm.h"


/*using namespace ftxui;

MainConsoleForm::MainConsoleForm() {
    // Initialize components
    auto readValueOneShotButton = ftxui::Button("[Read Value One Shot]", [&] {
        if (onReadValueOneShot) onReadValueOneShot();
    });

    auto readValuePollingButton = Button("[Read Value Polling]", [&] {
        if (onReadValuePolling) onReadValuePolling();
    });

    auto writeOutputButton = Button("[Write Output]", [&] {
        if (onWriteOutput) onWriteOutput();
    });


   


    auto selectModule = ftxui::Menu({"Module1", "Module2"});
    selectModule->on_change = [&] {
        if (onSelectModuleChanged) onSelectModuleChanged(selectedModuleIndex_);
    };

    auto selectChannel = Menu({"Channel1", "Channel2"}, &selectedChannelIndex_);
    selectChannel->on_change = [&] {
        if (onSelectChannelChanged) onSelectChannelChanged(selectedChannelIndex_);
    };

    // Layout
    auto layout = Container::Vertical({
        Container::Horizontal({
            selectModule,
            readValueOneShotButton,
        }),
        Container::Horizontal({
            selectChannel,
            readValuePollingButton,
        }),
        Container::Horizontal({
            writeOutputButton,
        }),
    });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            hbox({
                selectModule->Render() | flex,
                readValueOneShotButton->Render() | flex,
            }),
            hbox({
                selectChannel->Render() | flex,
                readValuePollingButton->Render() | flex,
            }),
            hbox({
                writeOutputButton->Render() | flex,
            }),
        });
    });

    screen_ = ScreenInteractive::TerminalOutput();
    screen_->AddComponent(renderer);
}

void MainConsoleForm::Show() {
    screen_->Loop();
}

void MainConsoleForm::initmoduleMenuOptions(std::vector<std::string> moduleAlaises)
{
}*/
