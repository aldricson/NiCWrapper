#include "QNiDaqWrapper.h"
#include <iostream>
#include <cstring>
#include "..\Ni modules definitions\NIDeviceModule.h"

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

    error = DAQmxCreateTask("getDevice", &taskHandle);

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


double QNiDaqWrapper::readCurrent(NIDeviceModule *deviceModule, unsigned int chanIndex, unsigned int maxRetries)
{
    if (deviceModule == nullptr)
    {
        throw std::invalid_argument("Null pointer passed for deviceModule.");
    }
    int32   error;
    float64 readValue;
    // Extract necessary information from the NIDeviceModule object
    const char* deviceName = deviceModule->getAlias().c_str();
    moduleShuntLocation shuntLoc = deviceModule->getModuleShuntLocation();
    if (shuntLoc == noShunt)
    {
        throw std::runtime_error("Failed current modules must have a shunt");
    }
    float64 shuntVal = deviceModule->getModuleShuntValue();
    double minRange  = deviceModule->getChanMin();
    double maxRange  = deviceModule->getChanMax();
    // extract channelName with its index
    const char* channelName = deviceModule->getChanNames()[chanIndex].c_str();
    std::string fullChannelName = std::string(deviceName) + std::string(channelName);

    unsigned int retryCount = 0;
    while (true) 
    {
        // Create a new task
        error = DAQmxCreateTask("getCurrentValue", &taskHandle);
        if (error) 
        {
            // Handle error
            throw std::runtime_error("Failed to create task for reading current.");
        }

        // DAQmxCreateAICurrentChan parameters in order:
        // TaskHandle taskHandle              : Handle to the task, used to identify the task in subsequent NI-DAQmx calls.
        // const char physicalChannel[]       : The name of the physical channel to use (e.g., "Mod1/ai0").
        // const char nameToAssignToChannel[] : Optional name to assign to the channel. We keep it empty.
        // int32 terminalConfig               : Terminal configuration for the channel. DAQmx_Val_Cfg_Default uses the default configuration.
        // float64 minVal                     : Minimum value you expect to measure (in Amperes).
        // float64 maxVal                     : Maximum value you expect to measure (in Amperes).
        // int32 units                        : Units to use, in this case, Amperes (DAQmx_Val_Amps).
        // int32 shuntResistorLoc             : Location of the shunt resistor. DAQmx_Val_Internal means it's internal to the device.
        // float64 extShuntResistorVal        : Value of the external shunt resistor (in Ohms). We use a default value of 249.0 Ohms.
        // const char customScaleName[]       : Name of a custom scale to apply to the channel. We don't use this here.

        error = DAQmxCreateAICurrentChan(taskHandle, 
                                         fullChannelName.c_str(), 
                                         "", 
                                         DAQmx_Val_Cfg_Default, 
                                         minRange / 1000.0, 
                                         maxRange / 1000.0, 
                                         DAQmx_Val_Amps, 
                                         shuntLoc, 
                                         shuntVal, 
                                         NULL);

        if (error) {
            handleErrorAndCleanTask();
            // Check if max retries reached
            if (++retryCount >= maxRetries) 
            {
                throw std::runtime_error("Failed to create channel after max retries.");
            }
            // Wait before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        break;  // Successfully created the channel, break out of the loop
    }


        // Start the task to begin sampling
    error = DAQmxStartTask(taskHandle);
    if (error)
    {
        // Handle error and clean up
        handleErrorAndCleanTask();
        throw std::runtime_error("Failed to start task for reading current.");
    }

    // Read the current value
    // DAQmxReadAnalogScalarF64 parameters:
    // TaskHandle taskHandle : Handle to the task
    // float64 timeout       : Time in seconds to wait for the function to read the value
    // float64 *value        : Pointer to the variable where the read value will be stored
    // bool32 *reserved      : Reserved for future use. Pass NULL (nullptr in C++).
    error = DAQmxReadAnalogScalarF64(taskHandle, 10.0, &readValue, nullptr);
    if (error)
    {
        // Handle error and clean up
        handleErrorAndCleanTask();
        throw std::runtime_error("Failed to start task for reading current.");
    }
    // Stop the task and clear it
    DAQmxStopTask(taskHandle);
    DAQmxClearTask(taskHandle);
    // Convert the read value to the appropriate unit (if necessary) and return it
    return static_cast<double>(readValue);

}

void QNiDaqWrapper::handleErrorAndCleanTask()
{
    char errBuff[2048] = {'\0'};
    DAQmxGetExtendedErrorInfo(errBuff, 2048);
    std::cerr << "Extended Error Info: " << errBuff << std::endl;
    DAQmxClearTask(taskHandle);
}
