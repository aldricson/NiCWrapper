#include "chooseShowValueMenu.h"

ChooseShowValueMenu::ChooseShowValueMenu(std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper)
{
    m_cfgWrapper = aConfigWrapper;
}

ChooseShowValueMenu::~ChooseShowValueMenu()
{
}

std::string ChooseShowValueMenu::displayChooseShowValueMenu()
{
    std::string choice;
    std::vector<std::string> options;
    options.push_back(" 0 .  Read one shot");
    options.push_back(" 1 .  Read loop");
    options.push_back(" x .  Previous Menu");
    clearConsole();
    displayMenu("Read Value", options); 
    std::cout << std::endl;
    std::cout << "Enter your choice: ";
    std::cin >> choice; 
    std::cin.clear();
    std::cin.ignore();
    return choice;
}
