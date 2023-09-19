#ifndef QNIDAQWRAPPER_H
#define QNIDAQWRAPPER_H

#include <vector>
#include <string>
#include <NIDAQmx.h>

class NIDeviceModule;

class QNiDaqWrapper {
public:
    QNiDaqWrapper();
    ~QNiDaqWrapper();

    int32 GetNumberOfModules();
    std::vector<std::string> GetDevicesList();
    double readCurrent(const char* deviceName, const char* channelName, float64 minRange, float64 maxRange);
    double readCurrent(NIDeviceModule *deviceModule, unsigned int chanIndex);

    
private:
    TaskHandle taskHandle;
};

#endif // QNIDAQWRAPPER_H
