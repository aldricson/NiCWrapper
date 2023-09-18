#ifndef QNISYSCONFIGWRAPPER_H
#define QNISYSCONFIGWRAPPER_H

#include <vector>
#include <string>
#include <NISysCfg.h>  // Include the NiSysCfg header
#include <nisyscfg.h>
#include <stdio.h>
#include <vector>
#include "../globals/globalConsts.h"
#include "../stringUtils/stringUtils.h"
#include "../Ni modules definitions/NIDeviceModule.h"
#include "../Ni modules definitions/NIDeviceModuleFactory.h"


class QNiSysConfigWrapper {
public:
    QNiSysConfigWrapper();
    ~QNiSysConfigWrapper();

    // Method to enumerate cRIO modules and their properties
    std::vector<std::string> EnumerateCRIOPluggedModules();
    NIDeviceModule * getModuleByIndex(size_t index);
    NIDeviceModule * getModuleBySlot(unsigned int slotNb);
    NIDeviceModule * getModuleByAlias(const std::string& alias);
    bool IsPropertyPresent(NISysCfgResourceHandle resourceHandle, NISysCfgResourceProperty propertyID);

     // Getter and Setter for moduleList
    std::vector<NIDeviceModule*> getModuleList() const;
    void setModuleList(const std::vector<NIDeviceModule*>& newModuleList);

    // Signal for moduleList changes
    std::function<void(const std::vector<NIDeviceModule*>&, QNiSysConfigWrapper*)> moduleListChangedSignal;


private:
   
   NISysCfgSessionHandle sessionHandle;       // Session handle
   std::vector<NIDeviceModule*> moduleList;
   
};


#endif // QNISYSCONFIGWRAPPER_H