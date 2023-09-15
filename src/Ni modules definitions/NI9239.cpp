#include "NI9239.h"


NI9239::NI9239() 
{
    initModule();  // Initialize the module with default values
}

void NI9239::initModule() 
{
    // Set default values
    m_nbChannel = 4;  // Assuming NI9239 has 4 channels, change as needed
    m_nbDigitalIoPort = 0 ;

    type = isAnalogicInputVoltage;  // Set the default module type
    chanNames.clear();  // Clear any existing channel names

    // Initialize channel names with default values
    for (int i = 0; i < 4; ++i)  // Assuming NI9239 has 4 channels, change as needed
    {
        chanNames.push_back("/a" + std::to_string(i));
    }

}

// The rest of the methods are the same as in NI9208
void NI9239::saveConfig() 
{
   NIDeviceModule::saveToFile("NI9239_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}


//***************  setters  ************
void NI9239::setChanNames(const std::vector<std::string>& names)
{
    chanNames = names;
}

void NI9239::loadConfig()
{
    NIDeviceModule::loadFromFile("NI9239_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}

//************* getters ************
std::vector<std::string> NI9239::getChanNames() const 
{
    return chanNames;
}


