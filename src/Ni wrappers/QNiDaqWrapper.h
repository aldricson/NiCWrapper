#ifndef QNIDAQWRAPPER_H
#define QNIDAQWRAPPER_H

#include <vector>
#include <string>
#include <NIDAQmx.h>
#include <iostream> 
#include <chrono>
#include <thread>
#include <functional>

class NIDeviceModule;

class QNiDaqWrapper {
public:
    QNiDaqWrapper();
    ~QNiDaqWrapper();

    int32 GetNumberOfModules();
    std::vector<std::string> GetDevicesList();
    double readCurrent(NIDeviceModule *deviceModule, unsigned int chanIndex, unsigned int maxRetries);
    void handleErrorAndCleanTask();

    //signals
    std::function<void(double lastValue,QNiDaqWrapper *sender)>    channelDataChangedSignal = nullptr;  //emited as soon as the data for a channel has changed, 
                                                                                                        //without any garanty that the channel is ready for a new task  
    std::function<void(double lastValue,QNiDaqWrapper *sender)>    channelDataReadySignal   = nullptr;  //emited when the task is fully done and the channel is ready for a new one
    
    

    //Getters and setters
        // Getter pour m_lastSingleChannelValue
    double getLastSingleChannelValue() const;
    // Setter pour m_lastSingleChannelValue
    void setLastSingleChannelValue(double value);
    //Call backs falling functions
    void handleTaskCompletion(int32 status);
    
private:
    TaskHandle taskHandle;
    double m_lastSingleChannelValue = 0.0; 
};




#endif // QNIDAQWRAPPER_H
