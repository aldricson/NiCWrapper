#include "NIDeviceModuleFactory.h"
#include "NI9208.h"
#include "NI9239.h"
#include "NI9423.h"
#include "NI9411.h"
#include "NI9481.h"
// Include other module headers here

NIDeviceModule* NIDeviceModuleFactory::createModule(const std::string& productName) {
    if (productName == "NI9208") {
        return new NI9208;
        //return std::make_unique<NI9208>();
    }
    else
        if (productName == "NI9239") {
        return new NI9239;
    }
    else
        if (productName == "NI9423") {
        return new NI9423;
    }
    else
        if (productName == "NI9411") {
        return new NI9411;
    }
        else
        if (productName == "NI9481") {
        return new NI9481;
    }
    // Add other product names and their corresponding classes here
    // else if (productName == "NI9215") {
    //     return std::make_unique<NI9215>();
    // }

    return nullptr; // or throw an exception if an unknown product name is passed
}