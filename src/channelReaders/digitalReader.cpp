#include "digitalReader.h"

// Constructor
DigitalReader::DigitalReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance,
                             std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
    : BaseReader(aSysConfigInstance, aDaqMxInstance)
{
    // Add any initialization specific to DigitalReader here
}


void DigitalReader::manualReadOneShot(const std::string &moduleAlias, const unsigned int &index, double &returnedValue)
{
    // Assuming each channel of NI9423 is a counter channel
    // Validate moduleAlias
    if (moduleAlias.empty()) {
        returnedValue = std::numeric_limits<double>::min();
        return;
    }

    // Fetch the device module by alias
    NIDeviceModule *deviceModule = m_sysConfig->getModuleByAlias(moduleAlias);
    if (!deviceModule) {
        returnedValue = std::numeric_limits<double>::min();
        return;
    }

    // Ensure the module is the correct type (digital input/counter)
    if (deviceModule->getModuleType() != isDigitalInput) {
        returnedValue = std::numeric_limits<double>::min();
        return;
    }

    try {
        // Read the counter value
        uint32_t countValue = 0;
        m_daqMx->readDigitalCounter(deviceModule, index, countValue);
        returnedValue = static_cast<double>(countValue);

        // Reset the counter for next reading
        m_daqMx->resetDigitalCounter(deviceModule, index);
    } catch (const std::exception& e) {
        // Error handling
        returnedValue = std::numeric_limits<double>::min();
    }
}


void DigitalReader::manualReadOneShot(const std::string &moduleAlias, const std::string &chanName, double &returnedValue) {
    if (moduleAlias.empty() || chanName.empty()) {
        returnedValue = std::numeric_limits<double>::min();
        return;
    }

    NIDeviceModule *deviceModule = m_sysConfig->getModuleByAlias(moduleAlias);
    if (!deviceModule) {
        returnedValue = std::numeric_limits<double>::min();
        return;
    }

    ModuleType modType = deviceModule->getModuleType();
    if (modType != isDigitalInput) {
        returnedValue = std::numeric_limits<double>::min();
        return;
    }

    try {
        // Assuming readDigitalCounter returns the count since last call or reset
        unsigned long long count = m_daqMx->readDigitalCounter(deviceModule, chanName);
        returnedValue = static_cast<double>(count);
        onOneShotValueReaded(returnedValue); // Notify value read
    }
    catch (const std::exception &e) {
        // Log exception message, if any
        onOneShotValueReaded(std::numeric_limits<double>::min());
        returnedValue = std::numeric_limits<double>::min();
    }
}
