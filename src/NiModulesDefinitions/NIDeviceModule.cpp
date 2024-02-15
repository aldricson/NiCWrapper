#include "NIDeviceModule.h"


NIDeviceModule::NIDeviceModule()

{
   m_ini = std::make_shared<IniObject>();
}

bool NIDeviceModule::checkAndLogEmptyFileName(const std::string &filename, const std::string& functionName)
{
    if (filename.empty()) 
    {
        std::cout << "************" << std::endl;
        std::cout << "*  ERROR : * Empty file name found in function: "<<functionName.c_str()<< std::endl;
        std::cout << "************" << std::endl;
        return false;
    }
    else
        return true;

}

void NIDeviceModule::logForReadIniError(const bool &status, const std::string &fieldInError, const std::string &fileName)
{
    if (!status)
    {
        std::cout << "************" << std::endl;
        std::cout << "*  ERROR : * Failed to read "<<fieldInError.c_str()<<" for module " << fileName.c_str() << std::endl;
        std::cout << "************" << std::endl;
    }
}

void NIDeviceModule::logCountError(const std::string &fieldInError, const std::string &fileName)
{
    std::cout << "***********" << std::endl;
    std::cout << "* ERROR : * Number of "<<fieldInError.c_str()<<" is zero, proceeding with default values for" << fileName.c_str() << std::endl;
    std::cout << "***********" << std::endl;
}

void NIDeviceModule::logMinMaxConsitencyError(const std::string &fileName)
{
    std::cout << "************" << std::endl;
    std::cout << "*  ERROR : * Consistency error in min max parameters (max is <= min) for module " << fileName.c_str() << std::endl;
    std::cout << "************" << std::endl; 
}

bool NIDeviceModule::loadModules(const std::string &filename, ModuleType &aModuleType)
{
    bool ok;
    // Ensure the filename is not empty
    if (!checkAndLogEmptyFileName(filename,"loadModules")) return false;
    // Read and set the module type
    int moduleType = m_ini->readInteger("modules", "type", static_cast<int>(m_moduleType), filename, ok);
    //if there's an error output it in the terminal
    logForReadIniError(ok,"type",filename);    
    // Directly casting to ModuleType enum. Ensure this cast is safe according to your enum definition.
    setModuleType(static_cast<ModuleType>(moduleType));
    aModuleType = m_moduleType;

    // Read and set the module name
    std::string moduleName = m_ini->readString("modules", "moduleName", m_moduleName, filename, ok);
    if (moduleName.empty() || !ok) 
    {
        //if there's an error output it in the terminal
        logForReadIniError(ok,"moduleName",filename); 
        return false;
    }
    setModuleName(moduleName);

    // Read and set the module alias
    std::string moduleAlias = m_ini->readString("modules", "alias", m_alias, filename,ok);
    if (moduleAlias.empty() || !ok) 
    {
        //if there's an error output it in the terminal
        logForReadIniError(ok,"alias",filename); 
        return false;
    }
    setAlias(moduleAlias);

    // Read and set the module shunt location
    
    int shuntLocation = m_ini->readInteger("modules", "shuntLocation", static_cast<int>(m_shuntLocation), filename, ok);
    if (!ok)
    {
        //if there's an error output it in the terminal
        logForReadIniError(ok,"shuntLocation",filename); 
    }
    // Directly casting to moduleShuntLocation enum. Ensure this cast is safe according to your enum definition.
    setModuleShuntLocation(static_cast<moduleShuntLocation>(shuntLocation));

    // Read and set the module shunt value
    double shuntValue = m_ini->readDouble("modules", "shuntValue", m_shuntValue, filename,ok);
    if (!ok)
    {
        //if there's an error output it in the terminal
        logForReadIniError(ok,"shuntValue",filename);
    }
    setModuleShuntValue(shuntValue);

    // Read and set the module terminal configuration
    int terminalConfig = m_ini->readInteger("modules", "terminalConfig", static_cast<int>(m_moduleTerminalConfig), filename,ok);
    if (!ok)
    {
        //if there's an error output it in the terminal
        logForReadIniError(ok,"terminalConfig",filename); ;
    }
    // Directly casting to moduleTerminalConfig enum. Ensure this cast is safe according to your enum definition.
    setModuleTerminalCfg(static_cast<moduleTerminalConfig>(terminalConfig));

    // Omitted setting 'moduleUnit' as it's not defined

    return true;
}


bool NIDeviceModule::loadChannels(const std::string &filename, const ModuleType &aModuleType) 
{
    bool readOk;
    // Check if the filename is empty
    if (!checkAndLogEmptyFileName(filename,"loadChannels")) return false;
     // Load the number of counters from the file
    unsigned int numChannels = m_ini->readUnsignedInteger("channels", "numberOfchannels", m_nbChannel, filename);
        
    if (numChannels == 0) 
    {
        if (aModuleType == ModuleType::isAnalogicInputCurrent || aModuleType == ModuleType::isAnalogicInputVoltage)
        {
            logCountError("channels",filename);
        }
        else
        {
            std::cout << "No channel for " << filename << std::endl;
        }
        // You can choose to return false here if zero counters is considered an error
    }
    // Reads and sets the number of channels from the specified file.
    setNbChannel(m_ini->readUnsignedInteger("channels", "numberOfChannels", m_nbChannel, filename));
    // Reads and sets the maximum analog channel value from the file.
    setChanMax(m_ini->readDouble("channels", "max", m_analogChanMax, filename,readOk));
    //if there's an error output it in the terminal
    logForReadIniError(readOk,"max",filename); 
    // Reads and sets the minimum analog channel value from the file.
    setChanMin(m_ini->readDouble("channels", "min", m_analogChanMin, filename,readOk));
    //if there's an error output it in the terminal
    logForReadIniError(readOk,"min",filename);  
    // Reads a vector of channel names from the file and checks if the operation was successful.
    bool ok = m_ini->readStringVector("channels", "channel", m_nbChannel, m_chanNames, filename);
    if (ok) 
    {
        // If reading the channel names was successful, return true.
        return true;
    } 
    else 
    {
        // If reading the channel names failed, log a message and return false.
        // std::cout << "Channels not loaded, default values initialized" << std::endl; // Debug message
        return false;
    }
}

bool NIDeviceModule::loadCounters(const std::string &filename, const ModuleType &aModuleType) 
{
    bool ok;
    // Check if the filename is empty
    if (!checkAndLogEmptyFileName(filename,"loadCounters")) return false;

    // Load the number of counters from the file
    unsigned int numCounters = m_ini->readUnsignedInteger("counters", "numberOfCounters", m_nbCounters, filename);
    if (numCounters == 0) 
    {
        if (aModuleType == ModuleType::isCounter)
        {
            logCountError("counters",filename);
        }
        else
        {
            std::cout << "No counters for " << filename << std::endl;
        }
        // You can choose to return false here if zero counters is considered an error
    }
    setNbCounters(numCounters);

    // Load the counter names
    bool namesOk = m_ini->readStringVector("counters", "counter", numCounters, m_counterNames, filename);
    if (!namesOk || m_counterNames.empty()) 
    {
        if (aModuleType == ModuleType::isCounter)
        {
            std::cout << "ERROR with " <<filename<< "software maybe instable"<< std::endl;
        }
    }

    // Load and set counter edge counting mode
    int edgeMode = m_ini->readInteger("counters", "edgeCountingMode", static_cast<int>(m_counterCountingEdgeMode), filename,ok);
    //if there's an error output it in the terminal
    logForReadIniError(ok,"edgeCountingMode",filename);
    // Additional validation can be added here based on the expected range of edgeMode
    setcounterCountingEdgeMode(static_cast<moduleCounterEdgeConfig>(edgeMode));

    // Load and set counter counting direction mode
    int countDirection = m_ini->readInteger("counters", "countingDirection", static_cast<int>(m_counterCountDirectionMode), filename,ok);
    logForReadIniError(ok,"countingDirection",filename);
    // Additional validation can be added here based on the expected range of countDirection
    setCounterCountDirectionMode(static_cast<moduleCounterMode>(countDirection));

    // Load and set counter max and min ensuring max is greater than or equal to min
    unsigned int counterMax = m_ini->readUnsignedInteger("counters", "countingMax", 4294967295, filename);
    unsigned int counterMin = m_ini->readUnsignedInteger("counters", "countingMin", 0, filename);
    if (counterMin > counterMax) 
    {
        std::cerr << "Error: Counter minimum value is greater than the maximum value." << std::endl;
        return false;
    }
    setCounterMax(counterMax);
    setCounterMin(counterMin);

    // If this point is reached, all data is successfully loaded
    return true;
}

bool NIDeviceModule::loadOutputs (const std::string &filename, const ModuleType &aModuleType)
{
    // Check if the filename is empty
    if (!checkAndLogEmptyFileName(filename,"loadOutputs")) return false;
     // Load the number of outputs from the file
    unsigned int numOutputs = m_ini->readUnsignedInteger("outputs", "numberOfOutputs", m_nbOutputs, filename);
    if (numOutputs == 0) 
    {
        if (aModuleType == ModuleType::isDigitalOutput)
        {
            logCountError("outputs",filename);
        }
        else
        {
            std::cout << "No outputs for " << filename << std::endl;
        }
       
    }
    setNbOutputs(numOutputs);

        // Load the counter names
    bool namesOk = m_ini->readStringVector("outputs", "output", numOutputs, m_digitalOutputNames, filename);
    if (!namesOk || m_digitalOutputNames.empty()) 
    {
        std::cout << "No output :";
        for (unsigned int  i=0; i<m_digitalOutputNames.size();++i)
        {
            std::cout << m_digitalOutputNames[i].c_str()<<" ; ";
        }
        std::cout << " for "<<filename << std::endl;

    }
    
    return true;
}


void NIDeviceModule::saveChannels(const std::string &filename) 
{
    // Check if the filename is empty
    if (!checkAndLogEmptyFileName(filename,"saveChannels")) return;

    // Check if the number of channels is valid
    if (m_nbChannel == 0 || m_chanNames.size() != m_nbChannel) 
    {
        std::cout << "Skipping saving channels for: " <<filename.c_str()<< std::endl;
    }
    else
    {
        // Write the number of channels, max and min values to the file
        m_ini->writeUnsignedInteger("channels", "numberOfChannels", m_nbChannel, filename);
        if (m_analogChanMax > m_analogChanMin) 
        {
            m_ini->writeDouble("channels", "max", m_analogChanMax, filename);
            m_ini->writeDouble("channels", "min", m_analogChanMin, filename);
        }
        else 
        {
            logMinMaxConsitencyError(filename);
        }
        // Save the channel names
        for (unsigned int i = 0; i < m_nbChannel; ++i) 
        {
            std::string key = "Channel" + std::to_string(i);
            // Check if channel name is not empty
            if (m_chanNames[i].empty()) 
            {
                std::cout << "Warning: Channel name at index " << i << " is empty." << std::endl;
                continue; // Skipping empty channel names
            }                
             m_ini->writeString("channels", key.c_str(), m_chanNames[i], filename);
        }
    }
    std::cout << "Channels saved for " << m_moduleName << "Success" << std::endl;
}


void NIDeviceModule::saveCounters(const std::string &filename)
{
    // Ensure the filename is not empty
    if (!checkAndLogEmptyFileName(filename,"saveCounters")) return;

    // Validate the number of counters
    if (m_nbCounters == 0 || m_counterNames.size() != m_nbCounters) 
    {
        std::cout << "Skipping saving counters for: " <<filename.c_str()<< std::endl;
    }
    else
    {
        // Write the number of counters to the file
        m_ini->writeUnsignedInteger("counters", "numberOfCounters", m_nbCounters, filename);
        // Iterate through counter names and save them
        for (unsigned int i = 0; i < m_nbCounters; ++i) 
        {
            std::string key = "counter" + std::to_string(i);
            // Skip saving empty counter names
            if (m_counterNames[i].empty()) 
            {
                std::cout << "Warning: Counter name at index " << i << " is empty." << std::endl;
                continue;
            }
            m_ini->writeString("counters", key.c_str(), m_counterNames[i], filename);
            // Write counter edge counting mode
            m_ini->writeInteger("counters", "edgeCountingMode", static_cast<int>(m_counterCountingEdgeMode), filename);
            m_ini->writeInteger("counters", "countingDirection", static_cast<int>(m_counterCountDirectionMode), filename);
            // Validate and write counter max and min values
            if (m_counterMax > m_counterMin) 
            {
                std::cerr << "Error: Counter maximum value is less than the minimum value." << std::endl;
                m_ini->writeUnsignedInteger("counters", "countingMax", m_counterMax, filename);
                m_ini->writeUnsignedInteger("counters", "countingMin", m_counterMin, filename);
            }
            else
            {
                logMinMaxConsitencyError(filename);
            }
        }
    }
    std::cout << "Counters saved for " << m_moduleName << "Success" << std::endl;
}


void NIDeviceModule::saveModules(const std::string &filename)
{
    // Check if the filename is empty
    if (!checkAndLogEmptyFileName(filename,"saveModules")) return;

    // Write the module type to the file
    // Removing the check against MAX_VALUE since it's not defined
    m_ini->writeInteger("modules", "type", static_cast<int>(m_moduleType), filename);

    // Write the module name to the file, checking for emptiness
    if (m_moduleName.empty()) {
        std::cerr << "Warning: Module name is empty." << std::endl;
    }
    m_ini->writeString("modules", "moduleName", m_moduleName, filename);

    // Write the alias to the file, allowing empty alias as it might not be critical
    m_ini->writeString("modules", "alias", m_alias, filename);

    // Write the shunt location to the file
    // Removing the check against MAX_VALUE since it's not defined
    m_ini->writeInteger("modules", "shuntLocation", static_cast<int>(m_shuntLocation), filename);

    // Write the shunt value to the file, assuming no specific validation required
    m_ini->writeDouble("modules", "shuntValue", m_shuntValue, filename);

    // Write the terminal configuration to the file
    // Removing the check against MAX_VALUE since it's not defined
    m_ini->writeInteger("modules", "terminalConfig", static_cast<int>(m_moduleTerminalConfig), filename);

    // Write the module unit to the file
    // Removing the check against MAX_VALUE since it's not defined
    m_ini->writeInteger("modules", "moduleUnit", static_cast<int>(m_moduleUnit), filename);

    // Optionally, log that modules are saved
    // std::cout << "Modules saved for " << m_moduleName << std::endl;
}


void NIDeviceModule::loadFromFile(const std::string &filename)
{
    
    // Check if the filename is empty before proceeding
    if (!checkAndLogEmptyFileName(filename,"loadFromFile")) return;
    ModuleType modType;
    // Boolean flags to track loading status
    std::cout<<"******** "<<filename.c_str()<<" ********"<<std::endl<<std::endl;
  
    bool modulesLoaded  = loadModules  (filename,modType);
    if (modulesLoaded) 
    {
        std::cout << "Modules information successfully loaded from the ini file." << std::endl;
    } 
    else 
    {
        std::cerr << "Failed to load modules information from the ini file." << std::endl;
    }
    bool channelsLoaded = loadChannels (filename,modType);
    bool countersLoaded = loadCounters (filename,modType);
    bool outputsLoaded  = loadOutputs  (filename,modType);
    

    if (modulesLoaded) 
    {
        std::cout << "Modules information successfully loaded from the ini file." << std::endl;
    } else {
        std::cerr << "Failed to load modules information from the ini file." << std::endl;
    }

    if (channelsLoaded) {
        std::cout << "Channels information successfully loaded from the ini file." << std::endl;
    } else {
        std::cerr << "Failed to load channel information from the ini file." << std::endl;
    }

    if (countersLoaded) {
        std::cout << "Counter information successfully loaded from the ini file." << std::endl;
    } else {
        std::cerr << "Failed to load counter information from the ini file." << std::endl;
    }

    if (outputsLoaded) {
        std::cout << "outputs information successfully loaded from the ini file." << std::endl;
    } else {
        std::cerr << "Failed to load counter information from the ini file." << std::endl;
    }



    // If any of the load functions failed, handle accordingly
    if (!channelsLoaded || !countersLoaded || !modulesLoaded) {
        // Handle the error, e.g., by setting a status flag or taking corrective action
        std::cerr << "One or more components failed to load properly." << std::endl;
        // Additional error handling code can be placed here
    }

    // Optionally, log overall success if all components loaded successfully
    if (channelsLoaded && countersLoaded && modulesLoaded) {
        std::cout << "All components successfully loaded from " << filename << std::endl;
    }
}


void NIDeviceModule::saveToFile(const std::string& filename) 
{
    return;
    // Check if the filename is empty before proceeding
    if (!checkAndLogEmptyFileName(filename,"saveToFile")) return;
    // Boolean flags to track saving status
    bool channelsSaved = true, countersSaved = true, modulesSaved = true;

    try 
    {
        saveChannels(filename);
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Exception occurred in saveChannels: " << e.what() << std::endl;
        channelsSaved = false;
    }

    try {
        saveCounters(filename);
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred in saveCounters: " << e.what() << std::endl;
        countersSaved = false;
    }

    try {
        saveModules(filename);
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred in saveModules: " << e.what() << std::endl;
        modulesSaved = false;
    }

    // If any of the save functions failed, handle accordingly
    if (!channelsSaved || !countersSaved || !modulesSaved) {
        // Handle the error, e.g., by setting a status flag or taking corrective action
        std::cerr << "One or more components failed to save properly." << std::endl;
        // Additional error handling code can be placed here
    }

    // Optionally, log overall success if all components saved successfully
    if (channelsSaved && countersSaved && modulesSaved) {
        std::cout << "All components successfully saved to " << filename << std::endl;
    }
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


void NIDeviceModule::setModuleType(ModuleType newType)
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

void NIDeviceModule::setNbOutputs(unsigned int newNbCounters)
{
    m_nbOutputs = newNbCounters;
    if (nbDigitalOutputsChangedSignal)
    {
        nbDigitalOutputsChangedSignal(m_nbOutputs,this);
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

ModuleType NIDeviceModule::getModuleType() const
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
