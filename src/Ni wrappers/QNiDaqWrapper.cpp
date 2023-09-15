#include "QNiDaqWrapper.h"
#include <iostream>
#include <cstring>

QNiDaqWrapper::QNiDaqWrapper() {
    taskHandle = 0;
}

QNiDaqWrapper::~QNiDaqWrapper() {
    if (taskHandle != 0) {
        DAQmxClearTask(taskHandle);
    }
}

int32 QNiDaqWrapper::GetNumberOfModules() {
    char devNames[512];
    int32 error = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devNames, sizeof(devNames));
    if (error) {
        // Handle error
        return -1;
    }

    char *token = strtok(devNames, ", ");
    int32 moduleCount = 0;

    while (token != NULL) {
        std::cout << "Found device: " << token << std::endl;
        moduleCount++;
        token = strtok(NULL, ", ");
    }
   
    return moduleCount;
}

std::vector<std::string> QNiDaqWrapper::GetDevicesList() {
    std::vector<std::string> devices;
    int32 error;
    uInt32 bufferSize = 0;

    error = DAQmxCreateTask("test", &taskHandle);

    if (error < 0) {
        std::cerr << "Unable to create new task" << std::endl;
        return devices;
    }

    error = DAQmxGetSysDevNames(nullptr, bufferSize);
    if (error < 0) {
        std::cerr << "Error determining buffer size for device names." << std::endl;
        DAQmxClearTask(taskHandle);
        return devices;
    }

    if (bufferSize == 0) {
        std::cerr << "Buffer size is 0!" << std::endl;
        DAQmxClearTask(taskHandle);
        return devices;
    }

    DAQmxClearTask(taskHandle);
    return devices;
}
