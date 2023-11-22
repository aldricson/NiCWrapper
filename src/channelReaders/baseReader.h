#ifndef BaseReader_H
#define BaseReader_H

#include <memory>
#include <functional>
#include <sstream>
#include <limits>
#include <atomic>
#include <termios.h>

#include "../NiWrappers/QNiSysConfigWrapper.h"
#include "../NiWrappers/QNiDaqWrapper.h"
#include "../NiModulesDefinitions/NIDeviceModule.h"
#include "../stringUtils/stringUtils.h"
#include "../Signals/KeyboardPoller.h"
#include "../timers/simpleTimer.h"
#include "../Menus/chooseModuleMenu.h"
#include "../Menus/chooseChannelMenu.h"
#include "../Menus/chooseShowValueMenu.h"

class BaseReader {
public:
    // Constructor 
    BaseReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance,
               std::shared_ptr<QNiDaqWrapper> aDaqMxInstance);
    // Destructor
    ~BaseReader();
    //raw data acquisition PURE VIRTUAL
    virtual void manualReadOneShot() = 0;
    virtual void manualReadOneShot(double &returnedValue) = 0;
    virtual void manualReadPolling() = 0;
    //ui
    virtual void displayChooseModuleMenu  ();
    virtual void displayChooseChannelMenu ();
    virtual void displayShowValueMenu     ();
    //acquisition 
    virtual void selectModuleAndChannel(const std::string& moduleName, const std::string& channelName);
    //public slots
    virtual void onPollingTimerTimeOut();
    virtual void onKeyboardHit(char key);
    virtual void onChannelDataReady (double lastValue,QNiDaqWrapper *sender);
    virtual void onOneShotValueReaded(double aValue);
    virtual void onChannelMenuSuccess();
    // Getters
    virtual std::shared_ptr<QNiSysConfigWrapper> getSysConfig() const;
    virtual std::shared_ptr<QNiDaqWrapper>       getDaqMx()     const;
    // Setters
    virtual void setSysConfig(const std::shared_ptr<QNiSysConfigWrapper>& newSysConfig);
    virtual void setDaqMx    (const std::shared_ptr<QNiDaqWrapper>&       newDaqMx    );
    // Signals
    std::function<void(std::shared_ptr<QNiSysConfigWrapper>, BaseReader* sender)> sysConfigChangedSignal  = nullptr;
    std::function<void(std::shared_ptr<QNiDaqWrapper>,       BaseReader* sender)> daqMxChangedSignal      = nullptr;
    std::function<void()> showMainMenuSignal = nullptr;


protected:
    std::shared_ptr<QNiSysConfigWrapper> m_sysConfig        ; //wrapper around NiSysConfig

    std::shared_ptr<QNiDaqWrapper>       m_daqMx            ; //wrapper around NiDaqMx
    std::shared_ptr<SimpleTimer>         m_pollingTimer     ; //timer to poll datas 
    std::shared_ptr<KeyboardPoller>      m_keyboardPoller   ; //to get events from keyboard
    std::shared_ptr<ChooseModuleMenu>    m_moduleMenu       ; //the menu to choose modules
    std::shared_ptr<ChooseChannelMenu>   m_channelMenu      ; //the menu to choose channel
    std::shared_ptr<ChooseShowValueMenu> m_valueMenu        ; //the menu to choose one shot or polling reading
    //thread safe polling tracking variables
    std::atomic<bool>          m_fromPolling; 
    std::atomic<bool>          mustStopPolling; 
    std::atomic<unsigned int>  m_manualPollCount;

    char            m_manuallySelectedModuleName[256] = ""      ;
    char            m_manuallySelectedChanName  [256] = ""      ;
    unsigned int    m_manuallySelectedChanIndex       =  0      ;
    NIDeviceModule *m_manuallySelectedModule          = nullptr ;
};


#endif // BaseReader_H
