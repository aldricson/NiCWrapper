#ifndef QNIDAQWRAPPER_H
#define QNIDAQWRAPPER_H

#include <vector>
#include <string>
#include <NIDAQmx.h>

class QNiDaqWrapper {
public:
    QNiDaqWrapper();
    ~QNiDaqWrapper();

    int32 GetNumberOfModules();
    std::vector<std::string> GetDevicesList();
    
private:
    TaskHandle taskHandle;
};

#endif // QNIDAQWRAPPER_H
