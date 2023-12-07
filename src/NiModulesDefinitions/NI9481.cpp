#include "NI9481.h"


NI9481::NI9481() 
{
    initModule();  // Initialize the module with default values
}

void NI9481::initModule() 
{
    // Set default values
    m_moduleName="NI9481";
    m_nbChannel  = 0;  // Assuming NI9481 has 4 channels, change as needed
    m_nbCounters = 0;

    m_nbDigitalIoPort = 1;
    m_nbDigitalOutputs = 4;
    m_moduleType = isDigitalOutput;  // Set the default module type
    m_counterNames       .clear();  // Clear any existing channel names
    m_digitalOutputNames .clear();
    std::string portPrefix="/port";
      
    for (unsigned int j = 0; j < m_nbDigitalIoPort;++j)
    {
        m_digitalIoNames.push_back(portPrefix+std::to_string(j));
        // Initialize channel names with default values
        for (unsigned int i = 0; i < m_nbDigitalOutputs; ++i)  // Assuming NI9481 has 4 channels, change as needed
        {
            m_digitalOutputNames.push_back(m_digitalIoNames[j] + "line/" + std::to_string(i));
        }
    }
    m_counterMin = 0;
    m_counterMax = 4294967295;
    m_shuntLocation = noShunt;
    m_shuntValue    = -999999.999;
    m_moduleTerminalConfig = noTerminalConfig;   
}

void NI9481::loadConfig()
{
    NIDeviceModule::loadFromFile("NI9481_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}

// The rest of the methods are the same as in NI9208
void NI9481::saveConfig() 
{
     NIDeviceModule::saveToFile("NI9481_"+std::to_string(NIDeviceModule::getSlotNb())+".ini");
}



