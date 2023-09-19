#include "NI9239.h"


NI9239::NI9239() 
{
    initModule();  // Initialize the module with default values
}

void NI9239::initModule() 
{
    // Set default values
    m_moduleName="NI9239";
    m_nbChannel       = 4;  // Assuming NI9239 has 4 channels, change as needed
    m_nbCounters      = 0;
    m_nbDigitalIoPort = 0 ;

    m_moduleType = isAnalogicInputVoltage;  // Set the default module type
    m_chanNames.clear();  // Clear any existing channel names

    // Initialize channel names with default values
    for (int i = 0; i < 4; ++i)  // Assuming NI9239 has 4 channels, change as needed
    {
        m_chanNames.push_back("/a" + std::to_string(i));
    }
    m_analogChanMax = 10.0;
    m_analogChanMin = -10.0;
    m_analogUnit    = "V";
    m_shuntLocation = noShunt;
    m_shuntValue    = -999999.999; 
}

// The rest of the methods are the same as in NI9208
void NI9239::saveConfig() 
{
   NIDeviceModule::saveToFile("NI9239_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}


void NI9239::loadConfig()
{
    NIDeviceModule::loadFromFile("NI9239_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}

//************* getters ************
std::vector<std::string> NI9239::getChanNames() const 
{
    return m_chanNames;
}


