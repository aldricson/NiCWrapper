#ifndef NIDEVICEMODULE_H
#define NIDEVICEMODULE_H

#include <vector>
#include <string>
#include <fstream>

enum moduleType
{
    isAnalogicInputCurrent = 0,
    isAnalogicInputVoltage = 1,
    isDigitalInputVoltage  = 2,
    isDigitalIO            = 8,
    isCounter              = 9,
    isDigitalIOAndCounter  = 10   
};

class NIDeviceModule {
protected:
    unsigned int nbChannel       = 16;
    unsigned int nbDigitalIoPort = 0 ;
    std::string  m_alias         = "";        
    std::vector<std::string> chanNames;
    moduleType type;

public:
    //virtuals
    virtual ~NIDeviceModule() {}
    virtual void loadFromFile(const std::string& filename);
    virtual void saveToFile  (const std::string& filename);


    virtual std::string getAlias();
    virtual void setAlias(const std::string& newAlias);


    //Pure virtuals 

    virtual void initModule()                       = 0;
    virtual unsigned int getNbChannel()             const = 0;
    virtual unsigned int getNbDigitalIOPorts()      const = 0;
    virtual std::vector<std::string> getChanNames() const = 0;
    virtual moduleType getModuleType()              const = 0;

    virtual void setNbChannel(unsigned int nb) = 0;
    virtual void setChanNames(const std::vector<std::string>& names) = 0;
    virtual void setModuleType(moduleType type) = 0;
     
    virtual void saveConfig()  = 0; 
};

#endif // NIDEVICEMODULE_H
