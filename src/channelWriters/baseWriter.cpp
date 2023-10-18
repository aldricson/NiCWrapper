#include "baseWriter.h"

BaseWriter::BaseWriter(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance, std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
{
    m_sysConfig = aSysConfigInstance; //the object to handle Ni configuration via NiSysConfig API
    m_daqMx     = aDaqMxInstance;     //the object that handles IO operations withs the devices (crio and its modules)
    m_moduleMenu = std::make_shared<ChooseModuleMenu>(m_sysConfig);
    m_moduleMenu->succesSignal = [this](){
                                            this->m_manuallySelectedModule     = this->m_moduleMenu->getSelectedModule();
                                            std::strncpy(m_manuallySelectedModuleName, m_manuallySelectedModule->getAlias().c_str(), sizeof(m_manuallySelectedModuleName) - 1);
                                            m_manuallySelectedModuleName[sizeof(m_manuallySelectedModuleName) - 1] = '\0';  // Null-terminate just in case 
                                            this->displayChooseChannelMenu();
                                         };

    m_moduleMenu->failedSignal = [this](){this->displayChooseModuleMenu();};  //equivalent to std::bind(&BaseReader::displayChooseModuleMenu,this)

    m_moduleMenu->backSignal   = [this](){
                                              if (showMainMenuSignal)
                                              {
                                                showMainMenuSignal();
                                              }
                                         };
}

BaseWriter::~BaseWriter()
{
}

void BaseWriter::displayChooseModuleMenu()
{
    // Display the module selection menu and handle the user's choice
    std::string choice = m_moduleMenu->displayChooseModuleMenu(filterMode::showAll,true);
    m_moduleMenu->handleChoice(choice);
}

void BaseWriter::displayChooseChannelMenu()
{
      //security checks
   if (!m_channelMenu)
   {
     std::cout<<"channel menu is nullptr: early return the sof may crash"<<std::endl;
     return;
   }

   if (!m_manuallySelectedModule)
   {
            std::cout << "Error in :displayChooseChannelMenu(), m_manuallySelectedModule is nullptr\n Press enter to try again." << std::endl;
            std::cin.get();
            std::cin.clear();
            std::cin.ignore();
            displayChooseModuleMenu();
            return;
    }         

   std::string choice = m_channelMenu->displayChooseChannelMenu(m_manuallySelectedModule);
   //security check
   if (choice == "") return;
   m_channelMenu->handleChoice(choice);
}


