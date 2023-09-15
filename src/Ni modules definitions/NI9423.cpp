#include "NI9423.h"


NI9423::NI9423() 
{
    initModule();  // Initialize the module with default values
}

void NI9423::initModule() 
{
    // Set default values
    m_nbChannel = 4;  // Assuming NI9423 has 4 channels, change as needed
    m_nbDigitalIoPort = 1;
    type = isDigitalIOAndCounter;  // Set the default module type
    chanNames.clear();  // Clear any existing channel names

    // Initialize channel names with default values
    for (int i = 0; i < 4; ++i)  // Assuming NI9423 has 4 channels, change as needed
    {
        chanNames.push_back("/ctr" + std::to_string(i));
    }
}

void NI9423::loadConfig()
{
    NIDeviceModule::loadFromFile("NI9423_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}

// The rest of the methods are the same as in NI9208
void NI9423::saveConfig() 
{
     NIDeviceModule::saveToFile("NI9423_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}



//***************  setters  ************


void NI9423::setChanNames(const std::vector<std::string>& names)
{
    chanNames = names;
}


//************* getters ************
std::vector<std::string> NI9423::getChanNames() const 
{
    return chanNames;
}

