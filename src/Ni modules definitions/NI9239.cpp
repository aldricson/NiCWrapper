#include "NI9239.h"


NI9239::NI9239() 
{
    initModule();  // Initialize the module with default values
    NIDeviceModule::loadFromFile("NI9239.ini");  // Load from file if exists, otherwise use default values
}

void NI9239::initModule() 
{
    // Set default values
    nbChannel = 4;  // Assuming NI9239 has 4 channels, change as needed
    nbDigitalIoPort = 0 ;

    type = isAnalogicInputVoltage;  // Set the default module type
    chanNames.clear();  // Clear any existing channel names

    // Initialize channel names with default values
    for (int i = 0; i < 4; ++i)  // Assuming NI9239 has 4 channels, change as needed
    {
        chanNames.push_back("/a" + std::to_string(i));
    }

        // Check if the file NI9208.ini exists using ifstream
    std::ifstream file("NI9239.ini");
    if (!file)
    {
        // If the file doesn't exist, call saveToFile to create it
        NIDeviceModule::saveToFile("NI9239.ini");
    }
}

// The rest of the methods are the same as in NI9208
void NI9239::saveConfig() 
{
   NIDeviceModule::saveToFile("NI9239.ini");
}


//***************  setters  ************
void NI9239::setNbChannel(unsigned int nb)
{
    nbChannel = nb;
}

void NI9239::setChanNames(const std::vector<std::string>& names)
{
    chanNames = names;
}

void NI9239::setModuleType(moduleType newType)
{
    type = newType;
}

//************* getters ************
unsigned int NI9239::getNbChannel() const 
{
    return nbChannel;
}

unsigned int NI9239::getNbDigitalIOPorts() const
{
    return nbDigitalIoPort;
}

std::vector<std::string> NI9239::getChanNames() const 
{
    return chanNames;
}

moduleType NI9239::getModuleType() const
{
    return type;
}
