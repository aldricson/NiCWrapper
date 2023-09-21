#ifndef QNIDAQWRAPPER_H
#define QNIDAQWRAPPER_H

#include <vector>
#include <string>
#include <NIDAQmx.h>
#include <iostream> 
#include <chrono>
#include <thread>

class NIDeviceModule;

class QNiDaqWrapper {
public:
    QNiDaqWrapper();
    ~QNiDaqWrapper();

    int32 GetNumberOfModules();
    std::vector<std::string> GetDevicesList();
    double readCurrent(NIDeviceModule *deviceModule, unsigned int chanIndex, unsigned int maxRetries);
    void handleErrorAndCleanTask();

    
private:
    TaskHandle taskHandle;
};

#endif // QNIDAQWRAPPER_H
