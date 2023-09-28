#include "QNiDaqWrapper.h"
#include <iostream>
#include <cstring>
#include "../NiModulesDefinitions/NIDeviceModule.h"

//NiDaqMx callbacks
static int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, 
                                      int32 status,
                                      void *callbackData)
{
    QNiDaqWrapper *daqWrapper = static_cast<QNiDaqWrapper*>(callbackData);
    daqWrapper->handleTaskCompletion(status);
    return 0;
}

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
        //std::cout << "Found device: " << token << std::endl;
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

    // Extract channelName with its index
    const char* channelName = deviceModule->getChanNames()[chanIndex].c_str();
    std::string fullChannelName = std::string(deviceName) + std::string(channelName);

    unsigned int retryCount = 0;
    while (true) 
    {
        // Create a new task
        error = DAQmxCreateTask("getCurrentValue", &taskHandle);
        if (error) 
        {
            handleErrorAndCleanTask();  // Handle error and clean up
            throw std::runtime_error("Failed to create task for reading current.");
        }

        // Register the Done callback before starting the task
        error = DAQmxRegisterDoneEvent(taskHandle, 0, DoneCallback, this);
        if (error)
        {
            handleErrorAndCleanTask();  // Handle error and clean up
            throw std::runtime_error("Failed to register Done callback.");
        }

        // Create an analog input current channel
        // DAQmxReadAnalogScalarF64 parameters:
        // TaskHandle taskHandle : Handle to the task
        // float64 timeout       : Time in seconds to wait for the function to read the value
        // float64 *value        : Pointer to the variable where the read value will be stored
        // bool32 *reserved      : Reserved for future use. Pass NULL (nullptr in C++).
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
            handleErrorAndCleanTask();  // Handle error and clean up
            if (++retryCount >= maxRetries) 
            {
                throw std::runtime_error("Failed to create channel after max retries.");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        break;  // Successfully created the channel, break out of the loop
    }

    // Start the task to begin sampling
    error = DAQmxStartTask(taskHandle);
    if (error)
    {
        handleErrorAndCleanTask();  // Handle error and clean up
        throw std::runtime_error("Failed to start task for reading current.");
    }

    // Read the current value
    error = DAQmxReadAnalogScalarF64(taskHandle, 10.0, &readValue, nullptr);
    if (error)
    {
        handleErrorAndCleanTask();  // Handle error and clean up
        throw std::runtime_error("Failed to read current value.");
    }

    // Stop the task
    error = DAQmxStopTask(taskHandle);
    if (error)
    {
        handleErrorAndCleanTask();  // Handle error and clean up
        throw std::runtime_error("Failed to stop task.");
    }

    // Clear the task to free resources
    DAQmxClearTask(taskHandle);

    // Convert the read value to the appropriate unit (if necessary) and return it
    double result = static_cast<double>(readValue);
    setLastSingleChannelValue(result);
    return result;
}


void QNiDaqWrapper::handleErrorAndCleanTask()
{
    char errBuff[2048] = {'\0'};
    DAQmxGetExtendedErrorInfo(errBuff, 2048);
    std::cerr << "Extended Error Info: " << errBuff << std::endl;
    DAQmxClearTask(taskHandle);
}

void QNiDaqWrapper::handleTaskCompletion(int32 status)
{
     std::cout<<"task complete"<<std::endl;
     if (status != 0) 
     {

        // Handle the error. You can use DAQmxGetExtendedErrorInfo here.
     }
    // Additional code to execute when the task is done.
        // Vérifiez si le signal est défini avant de l'émettre
    if (channelDataReadySignal) { 
        channelDataReadySignal(m_lastSingleChannelValue,this);
    }
}


// Getter pour m_lastSingleChannelValue
double QNiDaqWrapper::getLastSingleChannelValue() const {
    return m_lastSingleChannelValue;
}

// Setter pour m_lastSingleChannelValue
void QNiDaqWrapper::setLastSingleChannelValue(double value) {
    m_lastSingleChannelValue = value;


}
