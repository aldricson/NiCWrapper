#include "NI9208.h"


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
    m_counterCountingEdgeMode   = NoEdge;
    m_counterCountDirectionMode = NoCountMode;
    m_nbDigitalIoPort = 0;
    m_moduleType = isAnalogicInputCurrent;
    for (unsigned int i = 0; i < m_nbChannel; ++i)
    {
        m_chanNames.push_back("/ai" + std::to_string(i));
    }

    m_analogChanMax = mAmpsToAmps(20.0);
    m_analogChanMin = mAmpsToAmps(-20.0);
    m_moduleUnit    = Val_Amps;
    m_shuntLocation = defaultLocation;
    m_shuntValue    = 34.01;
    m_moduleTerminalConfig = referencedSingleEnded;
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

