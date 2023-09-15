#include "NI9423.h"


NI9423::NI9423() 
{
    initModule();  // Initialize the module with default values
    NIDeviceModule::loadFromFile("NI9423.ini");  // Load from file if exists, otherwise use default values
}

void NI9423::initModule() 
{
    // Set default values
    nbChannel = 4;  // Assuming NI9423 has 4 channels, change as needed
    nbDigitalIoPort = 1;
    type = isDigitalIOAndCounter;  // Set the default module type
    chanNames.clear();  // Clear any existing channel names

    // Initialize channel names with default values
    for (int i = 0; i < 4; ++i)  // Assuming NI9423 has 4 channels, change as needed
    {
        chanNames.push_back("/ctr" + std::to_string(i));
    }

        // Check if the file NI9208.ini exists using ifstream
    std::ifstream file("NI9423.ini");
    if (!file)
    {
        // If the file doesn't exist, call saveToFile to create it
       NIDeviceModule::saveToFile("NI9423.ini");
    }
}

// The rest of the methods are the same as in NI9208
void NI9423::saveConfig() 
{
     NIDeviceModule::saveToFile("NI9423.ini");
}



//***************  setters  ************
void NI9423::setNbChannel(unsigned int nb)
{
    nbChannel = nb;
}

void NI9423::setChanNames(const std::vector<std::string>& names)
{
    chanNames = names;
}

void NI9423::setModuleType(moduleType newType)
{
    type = newType;
}

//************* getters ************
unsigned int NI9423::getNbChannel() const 
{
    return nbChannel;
}

unsigned int NI9423::getNbDigitalIOPorts() const
{
    return nbDigitalIoPort;
}

std::vector<std::string> NI9423::getChanNames() const 
{
    return chanNames;
}

moduleType NI9423::getModuleType() const
{
    return type;
}
