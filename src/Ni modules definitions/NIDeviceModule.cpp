#include "NIDeviceModule.h"

void NIDeviceModule::loadFromFile(const std::string& filename)
{
    std::ifstream ini(filename);
    if (!ini.is_open())
    {
        fprintf(stderr,"Cannot open %s\n", filename.c_str());
        return;
    }

    std::string line;
    std::string section;
    while (std::getline(ini, line))
    {
        // Remove leading and trailing whitespaces
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        // Check for section headers
        if (line[0] == '[' && line[line.size() - 1] == ']')
        {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        // Parse key-value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos)
            continue; // Skip malformed lines

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Remove leading and trailing whitespaces from key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t;") + 1); 

        // Process the key-value pair based on the current section
        // Check if the current line belongs to the "Channel" section
        if (section == "Channel")
        {
            // If the key is "NumberOfChannels", it specifies the total number of channels
            if (key == "NumberOfChannels")
            {
                // Convert the value to an unsigned integer
                unsigned int n = std::stoi(value);

                // Update the number of channels using the setter method
                setNbChannel(n);

                // Resize the chanNames vector to match the new number of channels
                m_chanNames.resize(n);
            }
            // If the key starts with "Channel", it specifies the name of a channel
            else if (key.find("Channel") != std::string::npos)
            {
                // Extract the channel index from the key (e.g., "Channel7" -> index = 7)
                unsigned int index = std::stoi(key.substr(7));

                // Update the name of the channel at the extracted index, if it exists
                if (index < m_chanNames.size())
                {
                    m_chanNames[index] = value;
                }
            
            }
           else if (key.find("max chan Value"))
           {
               setChanMax(std::stod(value));
           }
           else if (key.find("min chan Value"))
           {
              setChanMin(std::stod(value)); 
           }
           else if (key.find("analog chan unit"))
           {
              setChanUnit(value);
           }
        }
        else if (section == "Counters")
        {
            if (key == "NumberOfCounters")
            {
                 // Convert the value to an unsigned integer
                unsigned int n = std::stoi(value);
                setNbCounters(n);
            }
                 // If the key starts with "Channel", it specifies the name of a channel
            else if (key.find("Counter") != std::string::npos)
            {
                 // Extract the channel index from the key (e.g., "Channel7" -> index = 7)
                unsigned int index = std::stoi(key.substr(7));

                // Update the name of the channel at the extracted index, if it exists
                if (index < m_counterNames.size())
                {
                    m_counterNames[index] = value;
                }
            
            } 

        }




        // Check if the current line belongs to the "Module" section
        else if (section == "Module")
        {
            // If the key is "Type", it specifies the type of the module
            if (key == "Type")
            {
                // Convert the value to an enum of type 'moduleType'
                moduleType newType = static_cast<moduleType>(std::stoi(value));

                // Update the module type using the setter method
                setModuleType(newType);
            }
        }
    }

    ini.close();
}

void NIDeviceModule::saveToFile(const std::string &filename)
{
     FILE* ini;
    if ((ini = fopen(filename.c_str(), "w")) == NULL) 
    {
        fprintf(stderr,"Cannot open %s\n", filename.c_str());
        return;
    }

    // Use a format string for fprintf
    fprintf(ini, "#\n# %s Configuration\n#\n\n[Channel]\n\n", filename.c_str());

    fprintf(ini, "NumberOfChannels = %u\n"  , m_nbChannel    );
    fprintf(ini, "min chan Value = %.3f\n"  , m_analogChanMin);
    fprintf(ini, "max chan Value = %.3f\n"  , m_analogChanMax);
    fprintf(ini ,"analog chan unit = %s\n"  , m_analogUnit.c_str());
    for (unsigned int i = 0; i < m_nbChannel; ++i) 
    {
        fprintf(ini, "Channel%d = %s ;\n", i, m_chanNames[i].c_str());
    }

    fprintf(ini, "\n[Counters]\n\n");
    fprintf(ini, "NumberOfCounters = %u\n"  , m_nbCounters    );
    for (unsigned int i = 0; i < m_nbCounters; ++i) 
    {
        fprintf(ini, "Counter%d = %s ;\n", i, m_counterNames[i].c_str());
    }


    fprintf(ini, "\n[Module]\n\nType = %d ;\n", static_cast<int>(getModuleType()));
    fprintf(ini, "Alias = %s\n", m_alias.c_str());
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
    type = newType;
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
        if (countersMaxChangedSignal)
        {
            countersMaxChangedSignal(m_counterMax,this);
        }
    }
}

void NIDeviceModule::setChanUnit(const std::string &newUnit)
{
    m_analogUnit = newUnit;
    if (chanUnitChangedSignal)
    {
        chanUnitChangedSignal(m_analogUnit,this);
    }
}

void NIDeviceModule::showModuleOnConsole() const
{

  std::cout<<"Enter showModuleOnConsole())"<<std::endl;
  std::cout<<std::endl<<m_moduleInfo.c_str()<<std::endl;
 
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
    return moduleType();
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

std::string NIDeviceModule::getChanUnit() const
{
    return m_analogUnit;
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


void NIDeviceModule::setSlotNb(unsigned int newSlot)
{
    m_slotNumber = newSlot;
    //if the signal is connected then emit it
    if (slotNumberChangedSignal) 
      {  // Check if the signal is connected to a slot
            slotNumberChangedSignal(newSlot,this);
      }

}
