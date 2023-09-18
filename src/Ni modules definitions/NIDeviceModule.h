#ifndef NIDEVICEMODULE_H
#define NIDEVICEMODULE_H

#include <vector>
#include <cstring>
#include <string.h>
#include <fstream>
#include <functional>
#include <iostream>

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
    unsigned int m_nbCounters      = 0;
    unsigned int m_nbDigitalIoPort = 0 ;
    unsigned int m_slotNumber      = 0 ;
    double       m_analogChanMin   = 0.0;
    double       m_analogChanMax   = 4.0;
    unsigned int m_counterMin      = 0;
    unsigned int m_counterMax      = 4294967295; //32 bits
    std::string  m_analogUnit      = "V";

    std::string  m_moduleName      = "";
    std::string  m_alias           = "";
    std::string  m_moduleInfo      = "";        
    std::vector<std::string> m_chanNames;
    std::vector<std::string> m_counterNames;
    moduleType type;

public:
    //virtuals
    virtual ~NIDeviceModule() {}
    virtual void loadFromFile(const std::string& filename);
    virtual void saveToFile  (const std::string& filename);


    virtual std::string getAlias();
    

    virtual void initModule()                          = 0;
    virtual unsigned int getNbChannel()                const;
    virtual unsigned int getNbCounters()               const;
    virtual unsigned int getSlotNb   ()                const;
    virtual unsigned int getNbDigitalIOPorts()         const;
    virtual std::string  getModuleInfo()               const;
    virtual std::vector<std::string> getChanNames()    const;
    virtual std::vector<std::string> getCounterNames() const;
    virtual moduleType  getModuleType()                const;
    virtual double      getChanMin   ()                const;
    virtual double      getChanMax   ()                const;
    virtual unsigned int getminCounters ()             const;
    virtual unsigned int getmaxCounters ()             const;
    virtual std::string getChanUnit  ()                const;   


    virtual void setNbChannel (unsigned int newNbChannels);
    virtual void setNbCounters(unsigned int newNbCounters);
    virtual void setNbDigitalIOPorts(unsigned int newNbPorts);
    virtual void setModuleInfo   (std::string newModuleInfo); 
    virtual void setSlotNb       (unsigned int newSlot);
    virtual void setAlias        (const std::string& newAlias);
    virtual void setChanNames    (const std::vector<std::string>& names);
    virtual void setCounterNames (const std::vector<std::string>& names);
    virtual void setModuleType   (moduleType newType);
    virtual void setChanMin      (double newChanMin);
    virtual void setChanMax      (double newChanMax);
    virtual void setCounterMin   (unsigned int newCountersMin);
    virtual void setCounterMax   (unsigned int newCountersMax);
    virtual void setChanUnit     (const std::string& newUnit);

    virtual void showModuleOnConsole() const;
     
    virtual void loadConfig()  = 0;
    virtual void saveConfig()  = 0; 

//**********************************
//***     PURE C++ SIGNALS      ****
//**********************************
    std::function<void(unsigned int,             NIDeviceModule *sender)>  nbChannelsChangedSignal           = nullptr;
    std::function<void(unsigned int,             NIDeviceModule *sender)>  nbCountersChangedSignal           = nullptr;
    std::function<void(unsigned int,             NIDeviceModule *sender)>  nbDigitalIoPortsChangedSignal     = nullptr;
    std::function<void(unsigned int,             NIDeviceModule *sender)>  slotNumberChangedSignal           = nullptr;
    std::function<void(std::string ,             NIDeviceModule *sender)>  moduleInfoChangedSignal           = nullptr;
    std::function<void(double      ,             NIDeviceModule *sender)>  chanMinChangedSignal              = nullptr; 
    std::function<void(double      ,             NIDeviceModule *sender)>  chanMaxChangedSignal              = nullptr;
    std::function<void(unsigned int,             NIDeviceModule *sender)>  countersMinChangedSignal          = nullptr; 
    std::function<void(unsigned int,             NIDeviceModule *sender)>  countersMaxChangedSignal          = nullptr;
    std::function<void(std::string,              NIDeviceModule *sender)>  chanUnitChangedSignal             = nullptr;
    std::function<void(std::vector<std::string>, NIDeviceModule *sender)>  chanNamesChangedSignal            = nullptr;    
    std::function<void(std::vector<std::string>, NIDeviceModule *sender)>  counterNamesChangedSignal         = nullptr;
};

#endif // NIDEVICEMODULE_H
