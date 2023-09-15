#ifndef NI9423_H
#define NI9423_H

#include "NIDeviceModule.h"
#include <vector>
#include <string>
#include <fstream>

class NI9423 : public NIDeviceModule {
private:


public:
    NI9423();

    void initModule()                       override;

    unsigned int getNbChannel()             const override;
    unsigned  int getNbDigitalIOPorts()     const override;  // Get the number of digital IO port
    std::vector<std::string> getChanNames() const override;
    moduleType getModuleType()              const override;

    void setNbChannel(unsigned int nb)                       override;
    void setChanNames(const std::vector<std::string>& names) override;
    void setModuleType(moduleType type)                      override;

    void saveConfig()  override;

};

#endif // NI9423_H
