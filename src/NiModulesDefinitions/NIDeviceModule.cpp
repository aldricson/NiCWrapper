#include "NIDeviceModule.h"

NIDeviceModule::NIDeviceModule()
{
   m_ini = std::make_shared<IniObject>();
}

bool NIDeviceModule::loadChannels(std::string filename) 
{
   setNbChannel(m_ini->readUnsignedInteger("Channels","NumberOfChannels",m_nbChannel,filename));
   setChanMax  (m_ini->readDouble("Channels","max",m_analogChanMax,filename));
   setChanMax  (m_ini->readDouble("Channels","min",m_analogChanMin,filename));
   bool ok = m_ini->readStringVector("channels","channel",m_nbChannel,m_chanNames,filename);
   if (ok)
    {
        //std::cout<<"Channels loaded: ok"<<std::endl;
        return true;
    }
    else
    {
        //std::cout<<"Channels not loaded, defautl values initialized"<<std::endl;
        return false;
    }
}

bool NIDeviceModule::loadCounters(std::string filename) 
{
    setNbCounters(m_ini->readUnsignedInteger("Counters","NumberOfCounters",m_nbCounters,filename));
    bool ok = m_ini->readStringVector("Counters","Counter",m_nbCounters,m_counterNames,filename);
   if (ok)
    {
        //std::cout<<"Counters loaded: ok"<<std::endl;
    }
    else
    {
        //std::cout<<"Counters not loaded, defautl values initialized"<<std::endl;
    }
    setcounterCountingEdgeMode   (static_cast<moduleCounterEdgeConfig>(m_ini->readInteger("Counters","edgeCountingMode" ,m_counterCountingEdgeMode,filename)));
    setCounterCountDirectionMode (static_cast<moduleCounterMode>      (m_ini->readInteger("Counters","countingDirection",m_counterCountDirectionMode,filename)));
    setCounterMax                (m_ini->readUnsignedInteger("Counters","countingMax",4294967295,filename));
    setCounterMin                (m_ini->readUnsignedInteger("Counters","countingMin",0,filename));
    // Successfully loaded all counters info
    return true;
}

bool NIDeviceModule::loadModules(std::string filename)
{
     // Add checks for required keys     
    setModuleType           (static_cast<moduleType>(m_ini->readInteger("Modules","type",m_moduleType,filename)));
    setModuleName           (m_ini->readString("Modules","moduleName",m_moduleName,filename));
    setAlias                (m_ini->readString("Modules","Alias",m_alias,filename));
    setModuleShuntLocation  (static_cast<moduleShuntLocation>(m_ini->readInteger("Modules","shuntLocation",m_shuntLocation,filename)));
    setModuleShuntValue     (m_ini->readDouble("Modules","shuntValue",m_shuntValue,filename));
    setModuleTerminalCfg    (static_cast<moduleTerminalConfig>(m_ini->readInteger("Modules","terminalConfig",m_moduleTerminalConfig,filename)));
    setModuleUnit           (static_cast<moduleUnit>          (m_ini->readInteger("Modules","moduleUnit",m_moduleUnit,filename)));
    return true;
}


void NIDeviceModule::saveChannels(std::string filename)
{
    //std::cout<<"entering save channels "<<m_moduleName<<std::endl;
    m_ini->writeUnsignedInteger("Channels", "NumberOfChannels", m_nbChannel,filename);
    m_ini->writeDouble("Channels", "max", m_analogChanMax,filename);
    m_ini->writeDouble("Channels", "min", m_analogChanMin,filename);

    // Save the channel names
    for (unsigned int i = 0; i < m_nbChannel; ++i)
    {
        std::string key = "Channel" + std::to_string(i);
        m_ini->writeString("Channels", key.c_str(), m_chanNames[i],filename);
    }
     //std::cout<<"channels saved"<<m_moduleName<<std::endl;
}

void NIDeviceModule::saveCounters(std::string filename)
{
   //std::cout<<"entering save counters "<<m_moduleName<<std::endl;
   m_ini->writeUnsignedInteger("Counters", "NumberOfCounters", m_nbCounters,filename);
   
   // Save the counter names
   for (unsigned int i = 0; i < m_nbCounters; ++i)
   {
       std::string key = "Counter" + std::to_string(i);
       m_ini->writeString("Counters", key.c_str(), m_counterNames[i],filename);
   }

   m_ini->writeInteger("Counters", "edgeCountingMode", static_cast<int>(m_counterCountingEdgeMode),filename);
   m_ini->writeInteger("Counters", "countingDirection", static_cast<int>(m_counterCountDirectionMode),filename);
   m_ini->writeUnsignedInteger("Counters", "countingMax", m_counterMax,filename);
   m_ini->writeUnsignedInteger("Counters", "countingMin", m_counterMin,filename);
   //std::cout<<"counters saved"<<m_moduleName<<std::endl;
}

void NIDeviceModule::saveModules(std::string filename)
{
   //std::cout<<"entering save modules "<<m_moduleName<<std::endl;
   m_ini->writeInteger("Modules", "type", static_cast<int>(m_moduleType),filename);
   m_ini->writeString("Modules", "moduleName", m_moduleName,filename);
   m_ini->writeString("Modules", "Alias", m_alias,filename);
   m_ini->writeInteger("Modules", "shuntLocation", static_cast<int>(m_shuntLocation),filename);
   m_ini->writeDouble("Modules", "shuntValue", m_shuntValue,filename);
   m_ini->writeInteger("Modules", "terminalConfig", static_cast<int>(m_moduleTerminalConfig),filename);
   m_ini->writeInteger("Modules", "moduleUnit", static_cast<int>(m_moduleUnit),filename);
  //std::cout<<"modules saved "<<m_moduleName<<std::endl;   
}




void NIDeviceModule::loadFromFile(const std::string &filename)
{ 
    if (!loadChannels(filename))
    {
        std::cerr << "Failed to load channel information from the ini file." << std::endl;   
    }
    if (!loadCounters(filename)) 
    {
            std::cerr << "Failed to load counter information from the ini file." << std::endl;
    }  
    if (!loadModules(filename)) 
    {
        std::cerr << "Failed to load modules information from the ini file." << std::endl;
    } 
}



void NIDeviceModule::saveToFile(const std::string& filename) {   
    saveChannels(filename);
    saveCounters(filename);
    saveModules (filename);
}


std::string NIDeviceModule::getAlias()
{
    return m_alias;
}

void NIDeviceModule::setAlias(const std::string &newAlias)
{
    m_alias=newAlias;
}

void NIDeviceModule::setChanNames(const std::vector<std::string> &names)
{
    m_chanNames = names;
    if (chanNamesChangedSignal)
    {
       chanNamesChangedSignal(m_chanNames,this);
    }
}

void NIDeviceModule::setCounterNames(const std::vector<std::string> &names)
{
    m_counterNames = names;
    if (counterNamesChangedSignal)
    {
        counterNamesChangedSignal(m_counterNames,this);
    }
}    


void NIDeviceModule::setModuleType(moduleType newType)
{
    m_moduleType = newType;
}

void NIDeviceModule::setChanMin(double newChanMin)
{
    if (newChanMin<m_analogChanMax)
    {
        m_analogChanMin = newChanMin;
        if (chanMinChangedSignal)
        {
            chanMinChangedSignal(m_analogChanMin,this);
        }
    }
}

void NIDeviceModule::setcounterCountingEdgeMode(moduleCounterEdgeConfig newCounterCountingEdgeMode)
{
    m_counterCountingEdgeMode = newCounterCountingEdgeMode;
    if (counterEdgeConfigChangedSignal)
    {
        counterEdgeConfigChangedSignal(newCounterCountingEdgeMode,this); 
    }
}

void NIDeviceModule::setCounterCountDirectionMode(moduleCounterMode newCounterCountMode)
{
    m_counterCountDirectionMode = newCounterCountMode;
    if (counterModeChangedSignal)
    {
        counterModeChangedSignal(newCounterCountMode,this);
    }
}

void NIDeviceModule::setChanMax(double newChanMax)
{
    if (newChanMax>m_analogChanMin)
    {
        m_analogChanMax = newChanMax;
        if (chanMinChangedSignal)
        {
            chanMinChangedSignal(m_analogChanMax,this);
        }
    }
}

void NIDeviceModule::setCounterMin(unsigned int newCountersMin)
{
    if (newCountersMin<m_counterMax)
    {
        m_counterMin=newCountersMin;
        if (countersMinChangedSignal)
        {
            countersMinChangedSignal(m_counterMin,this);
        }
    }
}



void NIDeviceModule::setCounterMax(unsigned int newCountersMax)
{
        if (newCountersMax>m_counterMin)
    {
        m_counterMax=newCountersMax;
        //perfectly equivalent to the Q_EMIT
        if (countersMaxChangedSignal)
        {
            countersMaxChangedSignal(m_counterMax,this);
        }
    }
}

void NIDeviceModule::setNbDigitalOutputs(unsigned int newNbDigitalOutputs)
{
    m_nbDigitalOutputs = newNbDigitalOutputs;
    if (nbDigitalOutputsChangedSignal)
    {
        nbDigitalOutputsChangedSignal(newNbDigitalOutputs,this);
    }
}

void NIDeviceModule::setModuleUnit(moduleUnit newUnit)
{
    m_moduleUnit = newUnit;
    if (chanUnitChangedSignal)
    {
        chanUnitChangedSignal(m_moduleUnit,this);
    }
}


std::string NIDeviceModule::getModuleName() const
{
  return m_moduleName;
}

unsigned int NIDeviceModule::getNbChannel() const
{
    return m_nbChannel;
}

unsigned int NIDeviceModule::getNbCounters() const
{
    return m_nbCounters;
}

unsigned int NIDeviceModule::getSlotNb() const
{
    return m_slotNumber;
}

unsigned int NIDeviceModule::getNbDigitalIOPorts() const
{
    return m_nbDigitalIoPort;
}

std::string NIDeviceModule::getModuleInfo() const
{
    return m_moduleInfo;
}

std::vector<std::string> NIDeviceModule::getChanNames() const
{
    return m_chanNames;
}

std::vector<std::string> NIDeviceModule::getCounterNames() const
{
    return m_counterNames;
}

moduleType NIDeviceModule::getModuleType() const
{
    return m_moduleType;
}

moduleShuntLocation NIDeviceModule::getModuleShuntLocation() const
{
    return m_shuntLocation;
}

moduleCounterEdgeConfig NIDeviceModule::getcounterCountingEdgeMode() const
{
    return m_counterCountingEdgeMode;
}

double NIDeviceModule::getModuleShuntValue() const
{
    return m_shuntValue;
}

moduleCounterMode NIDeviceModule::getCounterCountDirectionMode() const
{
    return m_counterCountDirectionMode;
}

unsigned int NIDeviceModule::getNbDigitalOutputs() const
{
    return m_nbDigitalOutputs;
}

moduleTerminalConfig NIDeviceModule::getModuleTerminalCfg() const
{
    return m_moduleTerminalConfig;
}

double NIDeviceModule::getChanMin() const
{
    return m_analogChanMin;
}

double NIDeviceModule::getChanMax() const
{
    return m_analogChanMax;
}

unsigned int NIDeviceModule::getminCounters() const
{
    return m_counterMin;
}

unsigned int NIDeviceModule::getmaxCounters() const
{
    return m_counterMax;
}

moduleUnit NIDeviceModule::getModuleUnit() const
{
    return m_moduleUnit;
}

void NIDeviceModule::setModuleName(const std::string &newModuleName)
{
    m_moduleName = newModuleName;
    if (moduleNameChangedSignal)
    {
        moduleNameChangedSignal(m_moduleName,this);
    }
}

void NIDeviceModule::setNbChannel(unsigned int newNbChannels)
{
    m_nbChannel = newNbChannels;
    //emit signal
    if (nbChannelsChangedSignal)
        {
            nbChannelsChangedSignal(newNbChannels,this);
        }
}

void NIDeviceModule::setNbCounters(unsigned int newNbCounters)
{
    m_nbCounters = newNbCounters;
    if (nbCountersChangedSignal)
    {
        nbCountersChangedSignal(m_nbCounters,this);
    }
}

void NIDeviceModule::setNbDigitalIOPorts(unsigned int newNbPorts)
{
    m_nbDigitalIoPort = newNbPorts;
    //emit signal
    if (nbDigitalIoPortsChangedSignal)
    {
       nbDigitalIoPortsChangedSignal(newNbPorts,this); 
    }
}

void NIDeviceModule::setModuleInfo(std::string newModuleInfo)
{
     m_moduleInfo = newModuleInfo;
        //emit signal
    if (moduleInfoChangedSignal)
    {
       moduleInfoChangedSignal(newModuleInfo,this); 
    }
    
}

void NIDeviceModule::setModuleShuntLocation(moduleShuntLocation newLocation)
{
    m_shuntLocation = newLocation;
    if (moduleShuntLocationChangedSgnal)
    {
        moduleShuntLocationChangedSgnal(m_shuntLocation,this);
    }
}

void NIDeviceModule::setModuleShuntValue(double newValue)
{
    m_shuntValue = newValue;
    if (moduleShuntValueChangedSignal)
    {
        moduleShuntValueChangedSignal(m_shuntValue,this);
    }
}

void NIDeviceModule::setModuleTerminalCfg(moduleTerminalConfig newTerminalConfig)
{
    m_moduleTerminalConfig = newTerminalConfig;
    if(moduleTerminalConfigChangedSignal)
    {
        moduleTerminalConfigChangedSignal(m_moduleTerminalConfig,this);
    }
}

void NIDeviceModule::setSlotNb(unsigned int newSlot)
{
    m_slotNumber = newSlot;
    //if the signal is connected then emit it
    if (moduleSlotNumberChangedSignal) 
      {  // Check if the signal is connected to a slot
            moduleSlotNumberChangedSignal(newSlot,this);
      }

}
