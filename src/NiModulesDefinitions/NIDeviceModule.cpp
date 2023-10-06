#include "NIDeviceModule.h"

void NIDeviceModule::loadFromFile(const std::string& filename) {
    IniParser(filename, [this](const std::string& section, const std::string& key, const std::string& value) {
        if (section == "Channel") {
            if (key == "NumberOfChannels") {
                unsigned int n = std::stoi(value);
                setNbChannel(n);
                m_chanNames.resize(n);
            } else if (key.find("Channel") != std::string::npos) {
                unsigned int index = std::stoi(key.substr(7));
                if (index < m_chanNames.size()) {
                    m_chanNames[index] = value;
                }
            } else if (key.find("max chan Value") != std::string::npos) {
                try {
                    setChanMax(std::stod(value));
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid argument for max chan Value: " << value << std::endl;
                }
            } else if (key.find("min chan Value") != std::string::npos) {
                try {
                    setChanMin(std::stod(value));
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid argument for min chan Value: " << value << std::endl;
                }
            }

        //---------- counters --------

        } else if (section == "Counters") {
            if (key == "NumberOfCounters") {
                unsigned int n = std::stoi(value);
                setNbCounters(n);
            } else if (key.find("Counter") != std::string::npos && getNbCounters() > 0) {
                unsigned int index = std::stoi(key.substr(7));
                if (index < m_counterNames.size()) {
                    m_counterNames[index] = value;
                }
              else if (key.find("edgeCountingMode") != std::string::npos) {
                moduleCounterEdgeConfig cm =static_cast<moduleCounterEdgeConfig>(std::stoi(value));
                setcounterCountingEdgeMode(cm);
              }
              else if (key.find("countingDirection") != std::string::npos) {
                moduleCounterMode  cd = static_cast<moduleCounterMode>(std::stoi(value));
                setCounterCountDirectionMode(cd);
              }
              else if (key.find("countingMin") != std::string::npos) {
                unsigned int cMin = std::stoi(value);
                setCounterMin(cMin);
              }
                else if (key.find("countingMax") != std::string::npos) {
                unsigned int cMax = std::stoi(value);
                setCounterMax(cMax);
              }
            }
            //---------- modules --------
        } else if (section == "Module") {
            if (key == "Type") {
                moduleType newType = static_cast<moduleType>(std::stoi(value));
                setModuleType(newType);
            } else if (key == "Module Name") {
                setModuleName(value);
            } else if (key == "Shunt Location") {
                try {
                    moduleShuntLocation newLocation = static_cast<moduleShuntLocation>(std::stoi(value));
                    setModuleShuntLocation(newLocation);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid argument for Shunt Location: " << value << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Out of range for Shunt Location: " << value << std::endl;
                }
            } else if (key.find("Shunt Value") != std::string::npos) {
                try {
                    setModuleShuntValue(std::stod(value));
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid argument for Shunt Value: " << value << std::endl;
                }
            } else if (key.find("Terminal Config") != std::string::npos) {
                moduleTerminalConfig newTerminalConfig = static_cast<moduleTerminalConfig>(std::stoi(value));
                setModuleTerminalCfg(newTerminalConfig);
            }
            else if (key.find("Module unit") != std::string::npos) {
                moduleUnit newModuleUnit = static_cast<moduleUnit>(std::stoi(value));
                setModuleUnit(newModuleUnit);
            }
        }
    });
}


void NIDeviceModule::saveToFile(const std::string &filename)
{
    FILE* ini;
    if ((ini = fopen(filename.c_str(), "w")) == NULL) 
    {
        fprintf(stderr,"Cannot open %s\n", filename.c_str());
        return;
    }

    fprintf(ini, "#\n# %s Configuration\n#\n\n[Channel]\n\n", filename.c_str());
    fprintf(ini, "NumberOfChannels = %u\n", m_nbChannel);
    fprintf(ini, "min chan Value = %.3f\n", m_analogChanMin);
    fprintf(ini, "max chan Value = %.3f\n", m_analogChanMax);
    for (unsigned int i = 0; i < m_nbChannel; ++i) 
    {
        fprintf(ini, "Channel%d = %s\n", i, m_chanNames[i].c_str());  
    }
    //-------------- Section counters ---------
    fprintf(ini, "\n[Counters]\n\n");                      //<--- this is how to write a new section in the ini file Note the syntax
    fprintf(ini, "NumberOfCounters = %u\n", m_nbCounters); //So all the others are keys of the sections UNTIL next section
    for (unsigned int i = 0; i < m_nbCounters; ++i) 
    {
        fprintf(ini, "Counter%d = %s\n", i, m_counterNames[i].c_str()); 
    }
    fprintf(ini, "edgeCountingMode = %u\n" , static_cast<int>(m_counterCountingEdgeMode  ));
    fprintf(ini, "countingDirection = %u\n", static_cast<int>(m_counterCountDirectionMode));
    fprintf(ini, "countingMin = %u\n"      , m_counterMin               );
    fprintf(ini, "countingMax = %u\n"      , m_counterMax               );
    //------------ Section Module ------------
    fprintf(ini, "\n[Module]\n\n");
    fprintf(ini, "Type = %d ;\n", static_cast<int>(getModuleType()));
    fprintf(ini, "Module Name = %s\n",m_moduleName.c_str());
    fprintf(ini, "Alias = %s\n", m_alias.c_str());
    fprintf(ini, "Shunt Location = %d\n",m_shuntLocation);
    fprintf(ini, "Shunt Value = %.3f\n", m_shuntValue);
    fprintf(ini, "Terminal Config = %d\n",static_cast<int>(m_moduleTerminalConfig));
    fprintf(ini ,"Module unit = %d\n", static_cast<int>(m_moduleUnit));
    fclose(ini);
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
    return m_counterCountDirectionMode
    ;
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
    if (slotNumberChangedSignal) 
      {  // Check if the signal is connected to a slot
            slotNumberChangedSignal(newSlot,this);
      }

}
