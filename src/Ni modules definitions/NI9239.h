#ifndef NI9239_H
#define NI9239_H

#include "NIDeviceModule.h"
#include <vector>
#include <string>
#include <fstream>

class NI9239 : public NIDeviceModule {
private:


public:
    NI9239();  // Constructor

    void initModule()                       override;  // Initialize the module with default values

    unsigned int getNbChannel()             const override;  // Get the number of channels
    unsigned  int getNbDigitalIOPorts()     const override;  // Get the number of digital IO port
    std::vector<std::string> getChanNames() const override;  // Get the names of the channels
    moduleType getModuleType()              const override;  // Get the type of the module

    void setNbChannel(unsigned int nb)                       override;  // Set the number of channels
    void setChanNames(const std::vector<std::string>& names) override;  // Set the names of the channels
    void setModuleType(moduleType type)                      override;  // Set the type of the module

    void saveConfig()  override ;  // Save the module configuration to a file
};

#endif // NI9239_H
