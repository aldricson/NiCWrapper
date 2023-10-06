#include "QNiDaqWrapper.h"
#include <iostream>
#include <cstring>
#include "../NiModulesDefinitions/NIDeviceModule.h"

//NiDaqMx callbacks
static int32 CVICALLBACK CurrentDoneCallback(TaskHandle taskHandle, 
                                      int32 status,
                                      void *callbackData)
{
    QNiDaqWrapper *daqWrapper = static_cast<QNiDaqWrapper*>(callbackData);
    daqWrapper->handleReadCurrentCompletion(status);
    return 0;
}

static int32 CVICALLBACK VoltageDoneCallback(TaskHandle taskHandle, 
                                      int32 status,
                                      void *callbackData)
{
    QNiDaqWrapper *daqWrapper = static_cast<QNiDaqWrapper*>(callbackData);
    daqWrapper->handleReadVoltageCompletion(status);
    return 0;
}

// Add a new callback for counter completion
static int32 CVICALLBACK CounterDoneCallback(TaskHandle taskHandle, 
                                      int32 status,
                                      void *callbackData)
{
    QNiDaqWrapper *daqWrapper = static_cast<QNiDaqWrapper*>(callbackData);
    daqWrapper->handleReadCounterCompletion(status);
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
    int32  termCfg   = deviceModule->getModuleTerminalCfg();
    int32  unit      = deviceModule->getModuleUnit();

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
        error = DAQmxRegisterDoneEvent(taskHandle, 0, CurrentDoneCallback, this);
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
                                         termCfg, 
                                         minRange / 1000.0, 
                                         maxRange / 1000.0, 
                                         unit, 
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
    setLastSingleCurrentChannelValue(result);
    return result;
}



double QNiDaqWrapper::readVoltage(NIDeviceModule *deviceModule, unsigned int chanIndex, unsigned int maxRetries)
{
    // Check for null pointer
    if (deviceModule == nullptr)
    {
        throw std::invalid_argument("Null pointer passed for deviceModule.");
    }
    // Declare variables
    int32 error;
    float64 readValue;
    TaskHandle taskHandle = 0;
    // Extract necessary information from NIDeviceModule
    const char* deviceName = deviceModule->getAlias().c_str();
    double minRange  = deviceModule->getChanMin();
    double maxRange  = deviceModule->getChanMax();
    int32 termCfg    = deviceModule->getModuleTerminalCfg();
    int32 unit       = deviceModule->getModuleUnit();
    // Construct the full channel name
    const char* channelName = deviceModule->getChanNames()[chanIndex].c_str();
    std::string fullChannelName = std::string(deviceName) + std::string(channelName);
    // Initialize retry count
    unsigned int retryCount = 0;
    // Loop to attempt channel creation
    while (true) 
    {
        // Create a new task
        error = DAQmxCreateTask("getVoltageValue", &taskHandle);
        if (error)
        {
            handleErrorAndCleanTask();  // Custom function to handle errors and clean up
            throw std::runtime_error("Failed to create task for reading voltage.");
        }
        // Register Done callback
        error = DAQmxRegisterDoneEvent(taskHandle, 0, VoltageDoneCallback, this);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to register Done callback.");
        }
        // Create an analog input voltage channel
        //parameters in calling order  
        //taskHandle	        The task to which to add the channels that this function creates.
        //physicalChannel	    The names of the physical channels to use to create virtual channels.
        //nameToAssignToChannel	The name(s) to assign to the created virtual channel(s).
        //terminalConfig	    The input terminal configuration for the channel.
        //minVal	            The minimum value, in units, that you expect to measure.
        //maxVal	            The maximum value, in units, that you expect to measure.
        //units	             	The units to use to return the voltage measurements.
        error = DAQmxCreateAIVoltageChan(taskHandle, 
                                         fullChannelName.c_str(), 
                                         "", 
                                         termCfg, 
                                         minRange, 
                                         maxRange, 
                                         unit, 
                                         NULL);
        if (error) 
        {
            handleErrorAndCleanTask();
            if (++retryCount >= maxRetries)
            {
                throw std::runtime_error("Failed to create channel after max retries.");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        // Add sample clock timing configuration (NEW)
        error = DAQmxCfgSampClkTiming(taskHandle, "", 1000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to set sample clock timing.");
        }

        // Start the task
        error = DAQmxStartTask(taskHandle);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to start task for reading voltage.");
        }
        // Read a voltage value
        error = DAQmxReadAnalogScalarF64(taskHandle, 10.0, &readValue, nullptr);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to read voltage value.");
        }
        // Stop the task
        error = DAQmxStopTask(taskHandle);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to stop task.");
        }
        // Clear the task to free up resources
        DAQmxClearTask(taskHandle);
        break; // If everything goes well, break the loop
    }

    // Convert to appropriate unit (if necessary)
    double result = static_cast<double>(readValue);
    setLastSingleVoltageChannelValue(result);  // I'm assuming this is a custom function you have

    return result;  // Return the read value
}


// Add a new method for reading counter values
unsigned int QNiDaqWrapper::readCounter(NIDeviceModule *deviceModule, unsigned int chanIndex, unsigned int maxRetries)
{
    // Check for null pointer
    if (deviceModule == nullptr)
    {
        throw std::invalid_argument("Null pointer passed for deviceModule.");
    }

    // Declare variables
    int32 error;
    uInt32 readValue;  // Counter values are usually integers
    TaskHandle taskHandle = 0;

    // Extract necessary information from NIDeviceModule
    const char* deviceName = deviceModule->getAlias().c_str();
    const char* channelName = deviceModule->getChanNames()[chanIndex].c_str();
    std::string fullChannelName = std::string(deviceName) + std::string(channelName);

    // Initialize retry count
    unsigned int retryCount = 0;

    // Loop to attempt channel creation
    while (true) 
    {
        // Create a new task for counter reading
        error = DAQmxCreateTask("getCounterValue", &taskHandle);
        if (error)
        {
            handleErrorAndCleanTask();  // Custom function to handle errors and clean up
            throw std::runtime_error("Failed to create task for reading counter.");
        }

        // Register Done callback for counter
        error = DAQmxRegisterDoneEvent(taskHandle, 0, CounterDoneCallback, this);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to register Done callback for counter.");
        }

        // Create a counter input channel
        error = DAQmxCreateCICountEdgesChan(taskHandle, 
                                            fullChannelName.c_str(), 
                                            "", 
                                            DAQmx_Val_Rising,  // Count rising edges (customize as needed)
                                            0,                 // Initial count
                                            DAQmx_Val_CountUp);// Counting direction
        if (error) 
        {
            handleErrorAndCleanTask();
            if (++retryCount >= maxRetries)
            {
                throw std::runtime_error("Failed to create counter channel after max retries.");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Start the task
        error = DAQmxStartTask(taskHandle);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to start task for reading counter.");
        }

        // Read a counter value
        error = DAQmxReadCounterScalarU32(taskHandle, 10.0, &readValue, nullptr);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to read counter value.");
        }

        // Stop the task
        error = DAQmxStopTask(taskHandle);
        if (error)
        {
            handleErrorAndCleanTask();
            throw std::runtime_error("Failed to stop counter task.");
        }

        // Clear the task to free up resources
        DAQmxClearTask(taskHandle);

        break; // If everything goes well, break the loop
    }

    // Convert to appropriate unit (if necessary)
    double result = static_cast<unsigned int>(readValue);
    setLastSingleCounterValue(result);  // Assuming you have a similar function for counter

    // Emit signal when counter data is ready
    if (channelCounterDataReadySignal) 
    { 
        channelCounterDataReadySignal(result, this);
    }

    return result;  // Return the read value
}




void QNiDaqWrapper::handleErrorAndCleanTask()
{
    char errBuff[2048] = {'\0'};
    DAQmxGetExtendedErrorInfo(errBuff, 2048);
    std::cerr << "Extended Error Info: " << errBuff << std::endl;
    DAQmxClearTask(taskHandle);
}

void QNiDaqWrapper::handleReadCurrentCompletion(int32 status)
{
     std::cout<<"task complete Current"<<std::endl;
     if (status != 0) 
     {

        // Handle the error. You can use DAQmxGetExtendedErrorInfo here.
     }
    // Additional code to execute when the task is done.
     // Verify if the signal is set befor emiting it
    if (channelCurrentDataReadySignal) 
    { 
        channelCurrentDataReadySignal(m_lastSingleCurrentChannelValue,this);
    }
}

void QNiDaqWrapper::handleReadVoltageCompletion(int32 status)
{
    std::cout<<"task complete Voltage"<<std::endl;
     if (status != 0) 
     {

        // Handle the error. You can use DAQmxGetExtendedErrorInfo here.
     }
    // Additional code to execute when the task is done.
     // Verify if the signal is set befor emiting it
    if (channelVoltageDataReadySignal) { 
        channelVoltageDataReadySignal(m_lastSingleVoltageChannelValue,this);
    }
}

void QNiDaqWrapper::handleReadCounterCompletion(int32 status)
{
     std::cout<<"task complete Counter"<<std::endl;
     if (status != 0) 
     {

        // Handle the error. You can use DAQmxGetExtendedErrorInfo here.
     }
    // Additional code to execute when the task is done.
     // Verify if the signal is set befor emiting it
    if (channelCounterDataReadySignal) { 
        channelCounterDataReadySignal(m_lastSingleCounter,this);
    }
}

// Getter pour m_lastSingleChannelValue
double QNiDaqWrapper::getLastSingleCurrentChannelValue() const {
    return m_lastSingleCurrentChannelValue;
}

// Setter pour m_lastSingleChannelValue
void QNiDaqWrapper::setLastSingleCurrentChannelValue(double value) {
    m_lastSingleCurrentChannelValue = value;
    //emited as soon as the data for a channel has changed, 
    if (channelCurrentDataChangedSignal)
    {
        channelCurrentDataChangedSignal(value,this);
    } 

}

double QNiDaqWrapper::getLastSingleVoltageChannelValue() const
{
    return m_lastSingleVoltageChannelValue;
}

void QNiDaqWrapper::setLastSingleVoltageChannelValue(double value)
{
    m_lastSingleVoltageChannelValue = value;
        //emited as soon as the data for a channel has changed, 
    if (channelVoltageDataChangedSignal)
    {
        channelVoltageDataChangedSignal(value,this);
    } 
}

unsigned int QNiDaqWrapper::getLastSingleCounterValue() const
{
    return m_lastSingleCounter;
}

void QNiDaqWrapper::setLastSingleCounterValue(unsigned int value)
{
    m_lastSingleCounter = value;
    if (channelCounterDataChangedSignal)
    {
        channelCounterDataChangedSignal(value,this);
    }
}
