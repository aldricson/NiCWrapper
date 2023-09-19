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

double QNiDaqWrapper::readCurrent(const char* deviceName, const char* channelName, float64 minRange, float64 maxRange)
{
    int32 error;
    float64 readValue;

    // Create a new task
    error = DAQmxCreateTask("", &taskHandle);
    if (error) 
    {
        // Handle error
        throw std::runtime_error("Failed to create task for reading current.");
    }

    // Create an analog input current channel
    error = DAQmxCreateAICurrentChan(taskHandle, 
                                     (std::string(deviceName) + "/" + channelName).c_str(), 
                                     "", 
                                     DAQmx_Val_Cfg_Default, 
                                     minRange, 
                                     maxRange, 
                                     DAQmx_Val_Amps, 
                                     DAQmx_Val_Internal, // Shunt Resistor Location (Internal for now)
                                     249.0, // External Shunt Resistor Value (default value)
                                     NULL); // Custom Scale Name
    if (error) {
        // Handle error
        DAQmxClearTask(taskHandle);
        throw std::runtime_error("Failed to create current channel.");
    }

    // Read the current value
    error = DAQmxReadAnalogScalarF64(taskHandle, 10.0, &readValue, NULL);
    if (error) {
        // Handle error
        DAQmxClearTask(taskHandle);
        throw std::runtime_error("Failed to read current.");
    }

    // Clear the task
    DAQmxClearTask(taskHandle);

    return readValue;
}

double QNiDaqWrapper::readCurrent(NIDeviceModule *deviceModule, unsigned int chanIndex)
{
    if (deviceModule == nullptr)
    {
        throw std::invalid_argument("Null pointer passed for deviceModule.");
    }

    int32 error;
    float64 readValue;

    // Extract necessary information from the NIDeviceModule object
    const char* deviceName = deviceModule->getModuleName().c_str();
    double minRange = deviceModule->getChanMin();
    double maxRange = deviceModule->getChanMax();
    // extract channelName with its index
    const char* channelName = deviceModule->getChanNames()[chanIndex].c_str();


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


    // Create an analog input current channel
    error = DAQmxCreateAICurrentChan(taskHandle, 
                                     (std::string(deviceName) + "/" + channelName).c_str(), 
                                     "", 
                                     DAQmx_Val_Cfg_Default, 
                                     minRange / 1000.0, //conversion in amps 
                                     maxRange / 1000.0, //converion in amps
                                     DAQmx_Val_Amps, 
                                     DAQmx_Val_Internal, // Shunt Resistor Location (Internal for now)
                                     249.0, // External Shunt Resistor Value (default value)
                                     NULL); // Custom Scale Name
}
