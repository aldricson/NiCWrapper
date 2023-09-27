#ifndef NIDEVICEMODULEFACTORY_H
#define NIDEVICEMODULEFACTORY_H

#include "NIDeviceModule.h"
#include <string>
#include <memory>

class NIDeviceModuleFactory {
public:
    static NIDeviceModule* createModule(const std::string& productName);
};

#endif // NIDEVICEMODULEFACTORY_H
