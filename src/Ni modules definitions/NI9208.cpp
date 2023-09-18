#include "NI9208.h"
//#include <filesystem>


NI9208::NI9208() 
{
    
   initModule();
}


void NI9208::initModule() 
{
    // Set default values
    m_moduleName="NI9208";
    m_nbChannel       = 16;
    m_nbCounters      = 0;
    m_nbDigitalIoPort = 0;
    type = isAnalogicInputCurrent;
    for (int i = 0; i < 16; ++i)
    {
        m_chanNames.push_back("/a" + std::to_string(i));
    }
    m_analogChanMax =  20.0;
    m_analogChanMin = -20.0;
    m_analogUnit    = "mA"; 
}


void NI9208::saveConfig() 
{
    NIDeviceModule::saveToFile("NI9208_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}

void NI9208::loadConfig()
{
        NIDeviceModule::loadFromFile("NI9208_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");

}





//************* getters ************
std::vector<std::string> NI9208::getChanNames() const 
{
    return m_chanNames;
}

