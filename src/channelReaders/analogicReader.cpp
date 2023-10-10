#include "analogicReader.h"

AnalogicReader::AnalogicReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance, std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
    : BaseReader(aSysConfigInstance, aDaqMxInstance) // Call the base class constructor
{
    // Your AnalogReader constructor code here
}

void AnalogicReader::manualReadOneShot()
{
    double junk;
    manualReadOneShot(junk);
}


void AnalogicReader::manualReadOneShot(double &returnedValue)
{
    if (!m_daqMx)
    {
        std::cout << "Error in :manualReadOneShot(), m_daqMxis nullptr\n Press enter to try again." << std::endl;
        std::cin.get();
        std::cin.clear();
        std::cin.ignore();
        displayChooseModuleMenu();  
    }
    
    
    if (!m_manuallySelectedModule)
    {
        std::cout << "Error in :manualReadOneShot(), m_manuallySelectedModule is nullptr\n Press enter to try again." << std::endl;
        std::cin.get();
        std::cin.clear();
        std::cin.ignore();
        displayChooseModuleMenu();
        return;   
    }

    moduleType modType = m_manuallySelectedModule->getModuleType();
    if (modType==isAnalogicInputCurrent)
    {
        double value;
        try
        {
            value = m_daqMx->readCurrent(m_manuallySelectedModule,m_manuallySelectedChanIndex,10,true);
            returnedValue = value;
        }
        catch(...)
        {
             onOneShotValueReaded(std::numeric_limits<double>::min());
             returnedValue = std::numeric_limits<double>::min();
        }    
        onOneShotValueReaded(value);
    }
    else if (modType==isAnalogicInputVoltage)
    {
        double value;
            try
            {
                value = m_daqMx->readVoltage(m_manuallySelectedModule,m_manuallySelectedChanIndex,10);
                returnedValue = value;
            }
            catch(...)
            {
                onOneShotValueReaded(std::numeric_limits<double>::min());
                returnedValue = std::numeric_limits<double>::min();
            }
            onOneShotValueReaded(value);
    }
}

void AnalogicReader::manualReadPolling()
{
    mustStopPolling.store(false);
    m_fromPolling.store(true);
    m_keyboardPoller->start();
    std::cout<<"keyBoardPoller started"<<std::endl;
    m_pollingTimer->start();
    std::cout<<"timer started"<<std::endl;
    while (mustStopPolling.load()!=true) 
    {
     // Maybe do some work or sleep
     std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    m_pollingTimer->stop(); 
    m_keyboardPoller->stop();
    displayShowValueMenu();
}

void AnalogicReader::displayChooseModuleMenu()
{
    // Display the module selection menu and handle the user's choice
    std::string choice = m_moduleMenu->displayChooseModuleMenu(filterMode::showOnlyReadAnalogics,true);
    m_moduleMenu->handleChoice(choice);
}
