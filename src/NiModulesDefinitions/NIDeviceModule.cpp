#include "NIDeviceModule.h"

void NIDeviceModule::loadFromFile(const std::string& filename) {
  // Check if the file exists the old way (crio system is outdated)
    std::ifstream fileCheck(filename);
    if (!fileCheck.good()) {
        // If the file doesn't exist, save the default configuration to the file
        saveToFile(filename);
    }
    fileCheck.close(); // Close the file stream
    IniParser parser(filename);

    // Channel section
    m_nbChannel = parser.readUnsignedInteger("Channel", "NumberOfChannels");
    m_analogChanMin = parser.readDouble("Channel", "min chan Value");
    m_analogChanMax = parser.readDouble("Channel", "max chan Value");
    m_chanNames = parser.readStringList("Channel", "Channel", m_nbChannel);

    // Counters section
    m_nbCounters = parser.readUnsignedInteger("Counters", "NumberOfCounters");
    m_counterNames = parser.readStringList("Counters", "Counter", m_nbCounters);
    m_counterCountingEdgeMode = static_cast<moduleCounterEdgeConfig>(parser.readInteger("Counters", "edgeCountingMode"));
    m_counterCountDirectionMode = static_cast<moduleCounterMode>(parser.readInteger("Counters", "countingDirection"));
    m_counterMin = parser.readUnsignedInteger("Counters", "countingMin");
    m_counterMax = parser.readUnsignedInteger("Counters", "countingMax");

    // Module section
    setModuleType(static_cast<moduleType>(parser.readInteger("Module", "Type")));
    m_moduleName = parser.readString("Module", "Module Name");
    m_alias = parser.readString("Module", "Alias");
    m_shuntLocation = static_cast<moduleShuntLocation>(parser.readInteger("Module", "Shunt Location"));  // Corrected this line
    m_shuntValue = parser.readDouble("Module", "Shunt Value");
    m_moduleTerminalConfig = static_cast<moduleTerminalConfig>(parser.readInteger("Module", "Terminal Config"));
    m_moduleUnit = static_cast<moduleUnit>(parser.readInteger("Module", "Module unit"));
}




void NIDeviceModule::saveToFile(const std::string &filename) {
    std::cout<<"enter save to file:"<<filename.c_str()<<std::endl;
    IniParser parser(filename);
   std::cout<<"We will never go here"<<std::endl;
    // Channel section
    parser.writeUnsignedInteger("Channel", "NumberOfChannels", m_nbChannel);
    parser.writeDouble("Channel", "min chan Value", m_analogChanMin);
    parser.writeDouble("Channel", "max chan Value", m_analogChanMax);
    parser.writeStringList("Channel", "Channel", m_chanNames);

    // Counters section
    parser.writeUnsignedInteger("Counters", "NumberOfCounters", m_nbCounters);
    parser.writeStringList("Counters", "Counter", m_counterNames);
    parser.writeInteger("Counters", "edgeCountingMode", static_cast<int>(m_counterCountingEdgeMode));
    parser.writeInteger("Counters", "countingDirection", static_cast<int>(m_counterCountDirectionMode));
    parser.writeUnsignedInteger("Counters", "countingMin", m_counterMin);
    parser.writeUnsignedInteger("Counters", "countingMax", m_counterMax);

    // Module section
    parser.writeInteger("Module", "Type", static_cast<int>(getModuleType()));
    parser.writeString("Module", "Module Name", m_moduleName);
    parser.writeString("Module", "Alias", m_alias);
    parser.writeInteger("Module", "Shunt Location", m_shuntLocation);
    parser.writeDouble("Module", "Shunt Value", m_shuntValue);
    parser.writeInteger("Module", "Terminal Config", static_cast<int>(m_moduleTerminalConfig));
    parser.writeInteger("Module", "Module unit", static_cast<int>(m_moduleUnit));
     //
     std::cout<<"parser ready to save"<<std::endl;
    // Save to file
    std::cout<<"ready to save:"<<filename.c_str()<<std::endl;
    parser.save();
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

void NIDeviceModule::showModuleOnConsole() const
{

  std::cout<<"Enter showModuleOnConsole())"<<std::endl;
  std::cout<<std::endl<<m_moduleInfo.c_str()<<std::endl;
 
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
