#include "baseReader.h"

//------- Creator --------

BaseReader::BaseReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance, std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
{
     //thread safe atomic variables
    m_manualPollCount.store(0);       //a counter for the polling
    m_fromPolling.store(false);       //a flag to track if some function called are from a polling mode or not
    m_sysConfig = aSysConfigInstance; //the object to handle Ni configuration via NiSysConfig API
    m_daqMx     = aDaqMxInstance;     //the object that handles IO operations withs the devices (crio and its modules)
    m_daqMx->channelCurrentDataReadySignal = std::bind(&BaseReader::onChannelDataReady, //Signal to Slot c++ style
                                              this,
                                              std::placeholders::_1,
                                              std::placeholders::_2);  //<-in that case 2 parameteres
    m_pollingTimer = std::make_shared<SimpleTimer>(); //The timer that handle polling
    m_pollingTimer -> setSlotFunction([this](){this-> onPollingTimerTimeOut();}); //passing the slot function to the calling function
    m_pollingTimer -> setInterval(std::chrono::milliseconds(500)); //500 ms interval between ticks
    m_keyboardPoller = std::make_shared<KeyboardPoller>();         //this object handle keyboards events while polling (e.g. escape key to stop polling) 
    
    m_keyboardPoller->keyboardHitSignal = std::bind(&BaseReader::onKeyboardHit,this,std::placeholders::_1); //signal to slot
    //menu to choose the modules + signals slots (mainly within lambdas)
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
    //menu to choose channels  + signals slots (mainly within lambdas)
    m_channelMenu = std::make_shared<ChooseChannelMenu>(m_sysConfig);
    //connect signal to slot   
    m_channelMenu->succesSignal = std::bind(&BaseReader::onChannelMenuSuccess,this);
    m_channelMenu->backSignal   = std::bind(&BaseReader::displayChooseChannelMenu,this);

    m_channelMenu->failedSignal = [this]() {
                                                if (showMainMenuSignal)
                                                   {
                                                       showMainMenuSignal();
                                                   }
                                                   else
                                                   {
                                                     std::cout<<"the soft will probably crash now"<<std::endl;
                                                     return;
                                                   }
                                           };
   m_valueMenu = std::make_shared<ChooseShowValueMenu>(m_sysConfig);
}

//---------- destructor -------------

BaseReader::~BaseReader()
{
}

//--------- ui ----------

void BaseReader::displayChooseModuleMenu()
{
    // Display the module selection menu and handle the user's choice
    std::string choice = m_moduleMenu->displayChooseModuleMenu(filterMode::showAll);
    m_moduleMenu->handleChoice(choice);
}

void BaseReader::displayChooseChannelMenu()
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

void BaseReader::displayShowValueMenu()
{
       std::cout<<"entering displayShowValueMenu()"<<std::endl; 
   std::string choice = m_valueMenu->displayChooseShowValueMenu();

    if (choice == "x" || choice == "X")
    {
        displayChooseModuleMenu();
    }
    else
    {
        unsigned int selectedAction;
        std::stringstream ss(choice);
        if (ss >> selectedAction && ss.eof()) 
        {
            switch (selectedAction)
            {
                case 0:
                {
                    m_fromPolling.store(false);
                    manualReadOneShot();
                    break;
                }
                case 1:
                {
                    m_fromPolling.store(true);
                    manualReadPolling();
                    break;
                }
            }
        }
    }
}

//--------- public slots ----------

void BaseReader::onPollingTimerTimeOut()
{
    // Call manualReadOneShot
    unsigned int i = m_manualPollCount.load();
    i++;
    m_manualPollCount.store(i);
    manualReadOneShot();
}

void BaseReader::onKeyboardHit(char key)
{
    if (key==27)
    {
        mustStopPolling.store(true);
    }
}

void BaseReader::onChannelDataReady(double lastValue, QNiDaqWrapper *sender)
{
    onOneShotValueReaded(lastValue);
}

void BaseReader::onOneShotValueReaded(double aValue)
{
    std::vector<std::string>           options;
    std::vector<std::function<void()>> actions;
    std::function<void()>              retryFunction;
    if (m_fromPolling.load())
    {
 
        clearConsole();
        std::string title;
        if (aValue==std::numeric_limits<double>::min())
        {
           title = "Read error";
        }
        else
        {
            title = "value: " + std::to_string(aValue) + " poll count:" + std::to_string(m_manualPollCount.load());
        }
       std::cout<<title.c_str()<<std::endl;
    }
    else
    {
        options.push_back(" 0 . Read again"             );    actions.push_back( [this](){m_fromPolling.store(false);
                                                                                         this->manualReadOneShot();} );
        options.push_back(" 1 . Choose another channel" );    actions.push_back( [this](){this->displayChooseChannelMenu();});  
        options.push_back(" 2 . Choose another module"  );    actions.push_back( [this](){this->displayChooseModuleMenu();});
        options.push_back(" 3 . Main Menu"              );    actions.push_back( [this](){  if (this->showMainMenuSignal) {this->showMainMenuSignal();}});
        retryFunction =  [this,aValue](){this->onOneShotValueReaded(aValue);};
        clearConsole();
        std::string title;
        if (aValue==std::numeric_limits<double>::min())
        {
           title = "Read error";
        }
        else
        {
           title = std::to_string(aValue);
        }
        displayMenu(title, options, actions, retryFunction); 
    }
}

void BaseReader::onChannelMenuSuccess()
{
    
    std::cout<<"Channel selection success"<<std::endl;
    //mostly over secured but who knows ?
    m_manuallySelectedChanIndex = m_channelMenu->selectedChannel;
    std::cout<<"Channel index selected"<<std::to_string(m_manuallySelectedChanIndex)<<std::endl;
    if (!m_manuallySelectedModule)
    {
        std::cout<<"slected module is nullptr!"<<std::endl;
       if (showMainMenuSignal)
       {
           showMainMenuSignal();
       }
       else
       {
         std::cout<<"the soft will probably crash now"<<std::endl;
         return;
       }
    }
    std::cout<<"Selected Module @"<<m_manuallySelectedModule<<std::endl; 
    //the active part :
    if (m_manuallySelectedChanIndex < m_manuallySelectedModule->getChanNames().size())   
    {
       std::cout<<"Selected Channel in range"<<std::endl;
       //get the channnel name from selected index, copy it in the destination char[256]
        std::strncpy(m_manuallySelectedChanName, 
                     m_manuallySelectedModule->getChanNames()[m_manuallySelectedChanIndex].c_str(),
                    sizeof(m_manuallySelectedChanName) - 1);
        //add terminal 0 (old school)
        m_manuallySelectedChanName[sizeof(m_manuallySelectedChanName) - 1] = '\0';  // Null-terminate just in case
        //assign the variables
        m_manuallySelectedChanIndex = m_channelMenu->selectedChannel;
        //Go to next menu/display
        displayShowValueMenu();
    } 
}

//----------- getters ---------------

std::shared_ptr<QNiSysConfigWrapper> BaseReader::getSysConfig() const
{
    return m_sysConfig;
}

std::shared_ptr<QNiDaqWrapper> BaseReader::getDaqMx() const
{
    return m_daqMx;
}

//-------------- setters ----------

void BaseReader::setSysConfig(const std::shared_ptr<QNiSysConfigWrapper> &newSysConfig)
{
    m_sysConfig = newSysConfig;
    if (sysConfigChangedSignal) {
        sysConfigChangedSignal(m_sysConfig, this);
    }
}

void BaseReader::setDaqMx(const std::shared_ptr<QNiDaqWrapper> &newDaqMx)
{
    m_daqMx = newDaqMx;
    if (daqMxChangedSignal) {
        daqMxChangedSignal(m_daqMx, this);
    }
}
