#ifndef NIDEVICEMODULE_H
#define NIDEVICEMODULE_H

#include <vector>
#include <string>
#include <fstream>
#include <functional>

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
    unsigned int m_nbChannel       = 16;
    unsigned int m_nbDigitalIoPort = 0 ;
    unsigned int m_slotNumber      = 0 ;
    std::string  m_moduleName    = "";
    std::string  m_alias         = "";        
    std::vector<std::string> chanNames;
    moduleType type;

public:
    //virtuals
    virtual ~NIDeviceModule() {}
    virtual void loadFromFile(const std::string& filename);
    virtual void saveToFile  (const std::string& filename);


    virtual std::string getAlias();
    

    virtual void initModule()                       = 0;
    virtual unsigned int getNbChannel()             const;
    virtual unsigned int getSlotNb   ()             const;
    virtual unsigned int getNbDigitalIOPorts()      const;
    virtual std::vector<std::string> getChanNames() const = 0;
    virtual moduleType getModuleType()              const;


    virtual void setNbChannel(unsigned int newNbChannels);
    virtual void setNbDigitalIOPorts(unsigned int newNbPorts);
    virtual void setSlotNb   (unsigned int newSlot);
    virtual void setAlias    (const std::string& newAlias);
    virtual void setChanNames(const std::vector<std::string>& names) = 0;
    virtual void setModuleType(moduleType newType);
     
    virtual void loadConfig()  = 0;
    virtual void saveConfig()  = 0; 

//**********************************
//***     PURE C++ SIGNALS      ****
//**********************************
    std::function<void(unsigned int, NIDeviceModule *sender)>  nbChannelsChangedSignal;
    std::function<void(unsigned int, NIDeviceModule *sender)>  nbDigitalIoPortsChangedSignal;
    std::function<void(unsigned int, NIDeviceModule *sender)>  slotNumberChangedSignal;  

};

#endif // NIDEVICEMODULE_H
