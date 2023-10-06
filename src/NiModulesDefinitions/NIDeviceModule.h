#ifndef NIDEVICEMODULE_H
#define NIDEVICEMODULE_H

#include <vector>
#include <cstring>
#include <string.h>
#include <fstream>
#include <functional>
#include <iostream>
#include "../filesUtils/iniParser.h"

#include "../config.h"
#ifdef CrossCompiled
  #include <NIDAQmx.h>
#else
  #include "../../DAQMX_INCLUDE/NIDAQmx.h"
#endif

enum moduleType
{
    isAnalogicInputCurrent = 0,
    isAnalogicInputVoltage = 1,
    isDigitalInput         = 2,
    isDigitalOutput        = 3,
    isDigitalIO            = 4,
    isCounter              = 5,
    isDigitalIOAndCounter  = 6   
};

enum moduleShuntLocation
{
    noShunt          =  0                  ,                    
    defaultLocation  = -1                  ,  //default
    internalLocation =  DAQmx_Val_Internal ,  //10200  Internal
    externalLocation =  DAQmx_Val_External    // 10167  External
};

enum moduleTerminalConfig
{
    noTerminalConfig          =  0                   ,
    defaultCfg                = -1                   ,
    referencedSingleEnded     = DAQmx_Val_RSE        ,  //10083  RSE
    nonReferencedSingleEnded  = DAQmx_Val_NRSE       ,  //10078  NRSE
    differencial              = DAQmx_Val_Diff       ,  //10106  Differential
    pseudoDifferencial        = DAQmx_Val_PseudoDiff    //12529  Pseudodifferential
};

enum moduleCounterEdgeConfig
{
    NoEdge      =  0                , 
    Val_Rising  = DAQmx_Val_Rising  , //10280 the counter will count uprising fronts
    Val_Falling = DAQmx_Val_Falling   //10171 the counter will count downfallinf fronts
};


enum moduleCounterMode
{
    NoCountMode         =   0                  ,
    Val_CountUp         =   DAQmx_Val_CountUp  ,         //10128  Count Up (Counter ++)
    Val_CountDown       =   DAQmx_Val_CountDown,        //10124  Count Down (Counter --)
    Val_ExtControlled   =   DAQmx_Val_ExtControlled     //10326   Externally Controlled
};


enum moduleUnit
{
    NoUnit                    =             0                                 ,
    Val_Volts                 =             DAQmx_Val_Volts                   ,                                                
    Val_Amps                  =             DAQmx_Val_Amps                    ,                               
    Val_DegF                  =             DAQmx_Val_DegF                    ,                               
    Val_DegC                  =             DAQmx_Val_DegC                    ,                               
    Val_DegR                  =             DAQmx_Val_DegR                    ,                               
    Val_Kelvins               =             DAQmx_Val_Kelvins                 ,                               
    Val_Strain                =             DAQmx_Val_Strain                  ,                               
    Val_Ohms                  =             DAQmx_Val_Ohms                    ,                               
    Val_Hz                    =             DAQmx_Val_Hz                      ,                               
    Val_Seconds               =             DAQmx_Val_Seconds                 ,                               
    Val_Meters                =             DAQmx_Val_Meters                  ,                               
    Val_Inches                =             DAQmx_Val_Inches                  ,                               
    Val_Degrees               =             DAQmx_Val_Degrees                 ,                               
    Val_Radians               =             DAQmx_Val_Radians                 ,                               
    Val_Ticks                 =             DAQmx_Val_Ticks                   ,                               
    Val_RPM                   =             DAQmx_Val_RPM                     ,                               
    Val_RadiansPerSecond      =             DAQmx_Val_RadiansPerSecond        ,                               
    Val_DegreesPerSecond      =             DAQmx_Val_DegreesPerSecond        ,                               
    Val_g                     =             DAQmx_Val_g                       ,                               
    Val_MetersPerSecondSquared=             DAQmx_Val_MetersPerSecondSquared  ,                               
    Val_InchesPerSecondSquared=             DAQmx_Val_InchesPerSecondSquared  ,                               
    Val_MetersPerSecond       =             DAQmx_Val_MetersPerSecond         ,                               
    Val_InchesPerSecond       =             DAQmx_Val_InchesPerSecond         ,                               
    Val_Pascals               =             DAQmx_Val_Pascals                 ,                               
    Val_Newtons               =             DAQmx_Val_Newtons                 ,                               
    Val_Pounds                =             DAQmx_Val_Pounds                  ,                               
    Val_KilogramForce         =             DAQmx_Val_KilogramForce           ,                               
    Val_PoundsPerSquareInch   =             DAQmx_Val_PoundsPerSquareInch     ,                               
    Val_Bar                   =             DAQmx_Val_Bar                     ,                               
    Val_NewtonMeters          =             DAQmx_Val_NewtonMeters            ,                               
    Val_InchOunces            =             DAQmx_Val_InchOunces              ,                               
    Val_InchPounds            =             DAQmx_Val_InchPounds              ,                               
    Val_FootPounds            =             DAQmx_Val_FootPounds              ,                               
    Val_VoltsPerVolt          =             DAQmx_Val_VoltsPerVolt            ,                               
    Val_mVoltsPerVolt         =             DAQmx_Val_mVoltsPerVolt           ,                               
    Val_Coulombs              =             DAQmx_Val_Coulombs                ,                               
    Val_PicoCoulombs          =             DAQmx_Val_PicoCoulombs            ,                               
    Val_FromTEDS              =             DAQmx_Val_FromTEDS                
};


class NIDeviceModule {
protected:
    unsigned int m_nbChannel       = 16;
    unsigned int m_nbCounters      = 0;
    unsigned int m_nbDigitalIoPort = 0 ;
    unsigned int m_slotNumber      = 0 ;
    double       m_analogChanMin   = 0.0;
    double       m_analogChanMax   = 4.0;
    
    unsigned int m_counterMin      = 0;
    unsigned int m_counterMax      = 4294967295; //32 bits
    moduleCounterEdgeConfig m_counterCountingEdgeMode;
    moduleCounterMode       m_counterCountDirectionMode;




    moduleUnit  m_moduleUnit       = NoUnit;

    std::string  m_moduleName      = "";
    std::string  m_alias           = "";
    std::string  m_moduleInfo      = "";        
    moduleShuntLocation  m_shuntLocation = defaultLocation;
    double               m_shuntValue    = 0.0;
    moduleTerminalConfig m_moduleTerminalConfig = noTerminalConfig;

    std::vector<std::string> m_chanNames;
    std::vector<std::string> m_counterNames;
    moduleType m_moduleType;

public:
    //virtuals
    virtual ~NIDeviceModule() {}
    virtual void loadFromFile(const std::string& filename);
    virtual void saveToFile  (const std::string& filename);


    virtual std::string getAlias();
    

    virtual void initModule()                          = 0;
    virtual std::string              getModuleName                () const;
    virtual unsigned int             getNbChannel                 () const;
    virtual unsigned int             getNbCounters                () const;
    virtual unsigned int             getSlotNb                    () const;
    virtual unsigned int             getNbDigitalIOPorts          () const;
    virtual std::string              getModuleInfo                () const;
    virtual std::vector<std::string> getChanNames                 () const;
    virtual std::vector<std::string> getCounterNames              () const;
    virtual moduleCounterEdgeConfig  getcounterCountingEdgeMode   () const; 
    virtual moduleCounterMode        getCounterCountDirectionMode () const;
    virtual moduleType               getModuleType                () const;
    virtual moduleShuntLocation      getModuleShuntLocation       () const;
    virtual double                   getModuleShuntValue          () const;
    virtual moduleTerminalConfig     getModuleTerminalCfg         () const;
    virtual moduleUnit               getModuleUnit                () const; 
    virtual double                   getChanMin                   () const;
    virtual double                   getChanMax                   () const;
    virtual unsigned int             getminCounters               () const;
    virtual unsigned int             getmaxCounters               () const;
      


    virtual void setModuleName                (const std::string& newModuleName);
    virtual void setNbChannel                 (unsigned int newNbChannels);
    virtual void setNbCounters                (unsigned int newNbCounters);
    virtual void setNbDigitalIOPorts          (unsigned int newNbPorts);
    virtual void setModuleInfo                (std::string newModuleInfo); 
    virtual void setModuleShuntLocation       (moduleShuntLocation newLocation);
    virtual void setModuleShuntValue          (double newValue);
    virtual void setModuleTerminalCfg         (moduleTerminalConfig newTerminalConfig); 
    virtual void setSlotNb                    (unsigned int newSlot);
    virtual void setAlias                     (const std::string& newAlias);
    virtual void setChanNames                 (const std::vector<std::string>& names                     );
    //-----------Counters--------
    virtual void setCounterNames              (const std::vector<std::string>& names                     );
    virtual void setcounterCountingEdgeMode   (moduleCounterEdgeConfig         newCounterCountingEdgeMode);
    virtual void setCounterCountDirectionMode (moduleCounterMode               newCounterCountMode       );
    virtual void setCounterMin                (unsigned int                    newCountersMin            );
    virtual void setCounterMax                (unsigned int                    newCountersMax            );

    
    virtual void setModuleType           (moduleType newType);
    virtual void setModuleUnit           (moduleUnit newUnit); 

    virtual void setChanMin              (double     newChanMin);
    virtual void setChanMax              (double newChanMax);

   

    virtual void showModuleOnConsole() const;
     
    virtual void loadConfig()  = 0;
    virtual void saveConfig()  = 0; 

//**********************************
//***     PURE C++ SIGNALS      ****
//**********************************
    std::function<void(std::string             , NIDeviceModule *sender)>  moduleNameChangedSignal           = nullptr;
    std::function<void(unsigned int            , NIDeviceModule *sender)>  nbChannelsChangedSignal           = nullptr;
    std::function<void(unsigned int            , NIDeviceModule *sender)>  nbCountersChangedSignal           = nullptr;
    std::function<void(unsigned int            , NIDeviceModule *sender)>  nbDigitalIoPortsChangedSignal     = nullptr;
    std::function<void(unsigned int            , NIDeviceModule *sender)>  slotNumberChangedSignal           = nullptr;
    std::function<void(std::string             , NIDeviceModule *sender)>  moduleInfoChangedSignal           = nullptr;
    std::function<void(double                  , NIDeviceModule *sender)>  chanMinChangedSignal              = nullptr; 
    std::function<void(double                  , NIDeviceModule *sender)>  chanMaxChangedSignal              = nullptr;
    std::function<void(unsigned int            , NIDeviceModule *sender)>  countersMinChangedSignal          = nullptr; 
    std::function<void(unsigned int            , NIDeviceModule *sender)>  countersMaxChangedSignal          = nullptr;
    std::function<void(moduleCounterEdgeConfig , NIDeviceModule *sender)>  counterEdgeConfigChangedSignal    = nullptr;
    std::function<void(moduleCounterMode       , NIDeviceModule *sender)>  counterModeChangedSignal          = nullptr;
    std::function<void(moduleUnit              , NIDeviceModule *sender)>  chanUnitChangedSignal             = nullptr;
    std::function<void(std::vector<std::string>, NIDeviceModule *sender)>  chanNamesChangedSignal            = nullptr;    
    std::function<void(std::vector<std::string>, NIDeviceModule *sender)>  counterNamesChangedSignal         = nullptr;
    std::function<void(moduleShuntLocation     , NIDeviceModule *sender)>  moduleShuntLocationChangedSgnal   = nullptr;
    std::function<void(double                  , NIDeviceModule *sender)>  moduleShuntValueChangedSignal     = nullptr;
    std::function<void(moduleTerminalConfig    , NIDeviceModule *sender)>  moduleTerminalConfigChangedSignal = nullptr;

};

#endif // NIDEVICEMODULE_H
