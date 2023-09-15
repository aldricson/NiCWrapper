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
    nbChannel = 16;
    nbDigitalIoPort = 0;
    type = isAnalogicInputCurrent;
    for (int i = 0; i < 16; ++i)
    {
        chanNames.push_back("/a" + std::to_string(i));
    }

    // Check if the file NI9208.ini exists using ifstream
    std::ifstream file("NI9208.ini");
    if (!file)
    {
        // If the file doesn't exist, call saveToFile to create it
        saveToFile("NI9208.ini");
    }
}


void NI9208::saveConfig() 
{
    NIDeviceModule::saveToFile("NI9208.ini");
}



//***************  setters  ************

void NI9208::setNbChannel(unsigned int nb)
{
    nbChannel = nb;
}

void NI9208::setChanNames(const std::vector<std::string>& names)
{
    chanNames = names;
}

void NI9208::setModuleType(moduleType newType)
{
    type = newType;
}

//************* getters ************

unsigned int NI9208::getNbChannel() const 
{
    return nbChannel;
}

unsigned int NI9208::getNbDigitalIOPorts() const
{
    return nbDigitalIoPort;
}

std::vector<std::string> NI9208::getChanNames() const 
{
    return chanNames;
}

moduleType NI9208::getModuleType() const
{
    return moduleType::isAnalogicInputCurrent;
}