#include "QNiSysConfigWrapper.h"
#include <iostream>
#include <cstring>



// Implementation of the constructor
QNiSysConfigWrapper::QNiSysConfigWrapper() 
{
    // Initialize session
    const char *login = "";
    const char *password = "";
    unsigned int timeOut = 1000; //1 second timeout  
    NISysCfgInitializeSession("localhost",login, password, NISysCfgLocaleDefault, NISysCfgBoolFalse, timeOut, NULL, &sessionHandle);
}


// Implementation of the destructor
QNiSysConfigWrapper::~QNiSysConfigWrapper() {
    // Close session handle
    NISysCfgCloseHandle(sessionHandle);
}



std::vector<std::string> QNiSysConfigWrapper::EnumerateCRIOPluggedModules() {
    std::vector<std::string>   modules;
    NISysCfgEnumResourceHandle resourceEnumHandle;

    char moduleName      [shortStringSize]; //product name of the module
    char moduleAlias     [shortStringSize]; //alias of the module e.g mod1
    unsigned int nb_chan          = 0;      //number of channels in the module
    unsigned int nb_counters      = 0;
    unsigned int nb_digitalIoPort = 0;      //number of digital io port in the module
    double       analogChanMin    = 0.0;
    double       analogChanMax    = 10.0;
    unsigned int counterMin       = 0.0;
    unsigned int counterMax       = 4294967295;
    std::string  analogUnits      = "";
    moduleType modType;
    int slotNumber;       //slot where is the module

    // Find hardware
    NISysCfgFindHardware(sessionHandle, NISysCfgFilterModeMatchValuesAll, NULL, NULL, &resourceEnumHandle);

    NISysCfgResourceHandle resourceHandle;
    int separatorCount = 0; //used as a crude way to isolate the part of interest
    while (NISysCfgNextResource(sessionHandle, resourceEnumHandle, &resourceHandle) == NISysCfg_OK) 
    {
        // Get the name of the module
        NISysCfgGetResourceProperty(resourceHandle, NISysCfgResourcePropertyProductName, moduleName);
        // Get the slot number
        NISysCfgGetResourceProperty(resourceHandle, NISysCfgResourcePropertySlotNumber, &slotNumber);
        //get the alias
        NISysCfgGetResourceIndexedProperty(resourceHandle, NISysCfgIndexedPropertyExpertUserAlias, 0, moduleAlias);
        
      

        if (std::strcmp(moduleName, "") == 0) {
            separatorCount++;
        }

        if (separatorCount >= 2) {
           
            removeSpacesFromCharStar(moduleName);
            std::string shortedModuleName = removeSpacesFromCharStar(moduleName);
             //output information for debug purpose
            std::string moduleInfo = std::string(moduleName) + 
                                     "\n║ Alias: "        + moduleAlias+
                                     "\n║ Slot: "         + std::to_string(slotNumber);
            //generate a device module object from the productname
            auto module = NIDeviceModuleFactory::createModule(shortedModuleName);
            if (module) 
            {
                //add to our list of modules
                moduleList.push_back(module);
                //set what we must
                module->setAlias(std::string(moduleAlias));
                module->setSlotNb(slotNumber);
                //load previous config if it exists (otherwise this will be default values of the module)
                module->loadConfig();
                //get what we need (or from the config file or default if it's the first run)
                nb_chan          = module->getNbChannel();
                nb_counters      = module->getNbCounters();
                modType          = module->getModuleType();
                nb_digitalIoPort = module->getNbDigitalIOPorts();
                analogChanMax    = module->getChanMax();
                analogChanMin    = module->getChanMin();
                analogUnits      = module->getChanUnit();
                counterMax       = module->getmaxCounters();
                counterMin       = module->getminCounters();

                //after setting the properties we could retrieve from NISysConfig
                //let's ensure our config files stay synchronized
                module->saveConfig();
                //convert it to string for debug purpose
                std::string modTypeAsString = "";
                switch (modType)
                {
                    case isAnalogicInputCurrent :
                    {
                        modTypeAsString = "\n║ type: Analog Input Current";
                        break;
                    }

                    case isAnalogicInputVoltage :
                    {
                        modTypeAsString = "\n║ type: Analog Input Voltage";
                        break;
                    }

                   case isDigitalInputVoltage :
                   {
                       modTypeAsString = "\n║ type: Digital Input Voltage";
                       break;
                   }

                   
                   case isDigitalIOAndCounter :
                   {
                       modTypeAsString = "\n║ type: Digital IO and counter";
                       break;
                   }

                   default :
                   {
                       modTypeAsString = "\n║ type: Not recognized";
                       break;
                   }

                } 
                moduleInfo += modTypeAsString+
                              "\n║ nb digital IO port: "  + 
                                std::to_string(nb_digitalIoPort) +
                              "\n║ nb channels: "  + 
                                std::to_string(nb_chan)+
                              "\n║ nb counters: "+
                               std::to_string(nb_counters);
                std::vector<std::string> channelNames = module->getChanNames();
                for (long unsigned int i=0;i<channelNames.size();++i)
                {
                   moduleInfo += "\n║ ╬"+ channelNames[i];
                }
                if (channelNames.size()>0)
                {
                    moduleInfo += "\n║ Analog min value  : "   + std::to_string(analogChanMin);
                    moduleInfo += "\n║ Analog max value  : "   + std::to_string(analogChanMax);
                    moduleInfo += "\n║ Analog input unit : "   + analogUnits;
                }

                std::vector<std::string> counterNames = module->getCounterNames();
                for (long unsigned int i=0;i<counterNames.size();++i)
                {
                   moduleInfo += "\n║ ╬"+ counterNames[i];
                }

                if (counterNames.size()>0)
                {
                    //in case of counters
                    moduleInfo += "\n║ 32 bits counters\n";
                    moduleInfo += "Counter Min Value : " + std::to_string(counterMin);
                    moduleInfo += "Counter Max Value : " + std::to_string(counterMax);
                }  
              module->setModuleInfo(moduleInfo);
            } 
            else 
            {
               moduleInfo += "\n║ Module inner definition not yet implemented";
            }
                                                
            modules.push_back(moduleInfo);
            
        }

        // Close the resource handle
        NISysCfgCloseHandle(resourceHandle);
    }

    // Close the resource enumeration handle
    NISysCfgCloseHandle(resourceEnumHandle);

    if (modules.size() >= 2) 
    {
        modules.erase(modules.begin(), modules.begin() + 2);
    } else {
        // Handle the case where there are fewer than 2 elements
    }

    return modules;
}


// Method to check if a property is present for a given resource
bool QNiSysConfigWrapper::IsPropertyPresent(NISysCfgResourceHandle resourceHandle, NISysCfgResourceProperty propertyID)
{
       NISysCfgIsPresentType isPresent;
        NISysCfgStatus status = NISysCfgGetResourceProperty(resourceHandle, propertyID, &isPresent);

        if (status == NISysCfg_OK) 
        {
            return (isPresent == NISysCfgIsPresentTypePresent);
        } 
        else 
        {
            std::cout << "An error occurred while checking for the device presence." << std::endl;
            // Handle error
            return false;
        }
}

//********************* getters **********************
// Function to get a module by its index
NIDeviceModule *  QNiSysConfigWrapper::getModuleByIndex(size_t index) {
    if (index >= moduleList.size()) {
        throw std::out_of_range("Index out of range");
    }
    return moduleList[index];
}

// Function to get a module by its alias
NIDeviceModule * QNiSysConfigWrapper::getModuleByAlias(const std::string& alias) {
    std::string lowerAlias = toLowerCase(alias);
    for (auto& module : moduleList) {
        if (toLowerCase(module->getAlias()) == lowerAlias) {  
            return module;
        }
    }
    throw std::invalid_argument("Module with given alias not found");
}


NIDeviceModule * QNiSysConfigWrapper::getModuleBySlot(unsigned int slotNb) {
        for (auto& module : moduleList) {
        if (module->getSlotNb() == slotNb) {  
            return module;
        }
    }
    throw std::invalid_argument("Module with given alias not found");

}

