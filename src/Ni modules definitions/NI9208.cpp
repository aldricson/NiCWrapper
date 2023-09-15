#include "NI9208.h"
//#include <filesystem>


NI9208::NI9208() 
{
    
   initModule();
     // Load from file if exists, otherwise use default values
    NIDeviceModule::loadFromFile("NI9208.ini");
}


void NI9208::initModule() 
{
    // Set default values
    m_nbChannel = 16;
    m_nbDigitalIoPort = 0;
    type = isAnalogicInputCurrent;
    for (int i = 0; i < 16; ++i)
    {
        chanNames.push_back("/a" + std::to_string(i));
    }


}


void NI9208::saveConfig() 
{
    NIDeviceModule::saveToFile("NI9208_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}

void NI9208::loadConfig()
{
        NIDeviceModule::loadFromFile("NI9208_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");

}


//***************  setters  ************

void NI9208::setChanNames(const std::vector<std::string>& names)
{
    chanNames = names;
}



//************* getters ************
std::vector<std::string> NI9208::getChanNames() const 
{
    return chanNames;
}

