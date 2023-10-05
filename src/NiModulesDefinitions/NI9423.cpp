#include "NI9423.h"


NI9423::NI9423() 
{
    initModule();  // Initialize the module with default values
}

void NI9423::initModule() 
{
    // Set default values
    m_moduleName="NI9423";
    m_nbChannel  = 0;  // Assuming NI9423 has 4 channels, change as needed
    m_nbCounters = 4;

    m_counterCountingEdgeMode   = Val_Rising;
    m_counterCountDirectionMode = Val_CountUp;

    m_nbDigitalIoPort = 1;
    m_moduleType = isDigitalIOAndCounter;  // Set the default module type
    m_counterNames.clear();  // Clear any existing channel names

    // Initialize channel names with default values
    for (unsigned int i = 0; i < m_nbCounters; ++i)  // Assuming NI9423 has 4 channels, change as needed
    {
        m_counterNames.push_back("/ctr" + std::to_string(i));
    }
    m_counterMin = 0;
    m_counterMax = 4294967295;
    m_shuntLocation = noShunt;
    m_shuntValue    = -999999.999;
    m_moduleTerminalConfig = noTerminalConfig;   
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



