#ifndef ANALOGICREADER_H
#define ANALOGICREADER_H

#include <memory>
#include <functional>
#include <sstream>
#include <limits>
#include <atomic>
#include <termios.h>

#include "..\Ni wrappers\QNiSysConfigWrapper.h"
#include "..\Ni wrappers\QNiDaqWrapper.h"
#include "..\Ni modules definitions\NIDeviceModule.h"
#include "..\stringUtils\stringUtils.h"
#include "..\Signals\KeyboardPoller.h"
#include "..\timers\simpleTimer.h"

class AnalogicReader {
private:
    std::shared_ptr<QNiSysConfigWrapper> m_sysConfig;
    std::shared_ptr<QNiDaqWrapper>       m_daqMx;
    std::shared_ptr<SimpleTimer>         m_pollingTimer;
    std::shared_ptr<KeyboardPoller>      m_keyboardPoller;

    char            m_manuallySelectedModuleName[256] = "";
    char            m_manuallySelectedChanName  [256] = "";
    unsigned int    m_manuallySelectedChanIndex       = 0;
    NIDeviceModule *m_manuallySelectedModule          = nullptr;
   // std::atomic<bool> timerActive = false;
    std::atomic<bool>          m_fromPolling; 
    std::atomic<bool>          mustStopPolling; 
    std::atomic<unsigned int>  m_manualPollCount;
    //private slots
    void onOneShotValueReaded(double aValue);
    

public:


    // Constructors and destructors
    AnalogicReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance,
                   std::shared_ptr<QNiDaqWrapper> aDaqMxInstance);
    virtual ~AnalogicReader();

    void displayChooseModuleMenu();
    void displayChooseChannelMenu();
    void displayShowValueMenu();
    void manualReadOneShot();
    void manualReadPolling();

    //public slots
    void onPollingTimerTimeOut();
    void onKeyboardHit(char key);
    void onChannelDataReady (double lastValue,QNiDaqWrapper *sender);


    // Getters
    std::shared_ptr<QNiSysConfigWrapper> getSysConfig() const;
    std::shared_ptr<QNiDaqWrapper> getDaqMx() const;

    // Setters
    void setSysConfig(const std::shared_ptr<QNiSysConfigWrapper>& newSysConfig);
    void setDaqMx(const std::shared_ptr<QNiDaqWrapper>& newDaqMx);

    // Signals
    std::function<void(std::shared_ptr<QNiSysConfigWrapper>, AnalogicReader* sender)> sysConfigChangedSignal  = nullptr;
    std::function<void(std::shared_ptr<QNiDaqWrapper>,       AnalogicReader* sender)> daqMxChangedSignal      = nullptr;
    std::function<void()> showMainMenuSignal = nullptr;


};




#endif // ANALOGICREADER_H
