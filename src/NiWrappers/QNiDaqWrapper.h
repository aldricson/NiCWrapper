#ifndef QNIDAQWRAPPER_H
#define QNIDAQWRAPPER_H

#include <vector>
#include <string>

#include <iostream> 
#include <chrono>
#include <thread>
#include <functional>

#include "../config.h"
#ifdef CrossCompiled
  #include <NIDAQmx.h>
#else
  #include "../../DAQMX_INCLUDE/NIDAQmx.h"
#endif

class NIDeviceModule;

class QNiDaqWrapper {
public:
    QNiDaqWrapper();
    ~QNiDaqWrapper();

    int32 GetNumberOfModules();
    std::vector<std::string> GetDevicesList();
    double readCurrent(NIDeviceModule *deviceModule, unsigned int chanIndex, unsigned int maxRetries);
    double readVoltage(NIDeviceModule *deviceModule, unsigned int chanIndex, unsigned int maxRetries);
    void handleErrorAndCleanTask();

    //signals
    std::function<void(double lastValue,QNiDaqWrapper *sender)>    channelCurrentDataChangedSignal = nullptr;  //emited as soon as the data for a channel has changed, 
                                                                                                               //without any garanty that the channel is ready for a new task  
    std::function<void(double lastValue,QNiDaqWrapper *sender)>    channelCurrentDataReadySignal   = nullptr;  //emited when the task is fully done and the channel is ready for a new one
    
    std::function<void(double lastValue,QNiDaqWrapper *sender)>    channelVoltageDataChangedSignal = nullptr;
    std::function<void(double lastValue, QNiDaqWrapper *sender)>   channelVoltageDataReadySignal   = nullptr;
    

    //Getters and setters
    double getLastSingleCurrentChannelValue() const;
    void   setLastSingleCurrentChannelValue(double value);
    double getLastSingleVoltageChannelValue() const;
    void   setLastSingleVoltageChannelValue(double value);
    //Call backs falling functions
    void handleReadCurrentCompletion(int32 status);
    void handleReadVoltageCompletion(int32 status);
    
private:
    TaskHandle taskHandle;
    double m_lastSingleCurrentChannelValue = 0.0;
    double m_lastSingleVoltageChannelValue = 0.0; 
};


#endif // QNIDAQWRAPPER_H
