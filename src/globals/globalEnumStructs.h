#ifndef GLOBALENUMSTRUCTS_H
#define GLOBALENUMSTRUCTS_H

#include "../config.h"
#ifdef CrossCompiled
  #include <NIDAQmx.h>
#else
  #include "../../DAQMX_INCLUDE/NIDAQmx.h"
#endif


enum class ModuleIo {
    INPUT = 0,
    OUTPUT = 1
};


enum ModuleType
{
    errorOrMissingModule   =-1,          
    isAnalogicInputCurrent = 0,
    isAnalogicInputVoltage = 1,
    isDigitalInput         = 2,
    isDigitalOutput        = 3,
    isCounter              = 4,
    isCoder                = 5 
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

struct MappingConfig {
    int index;
    ModuleType moduleType;
    std::string module;
    std::string channel;
    float minSource;
    float maxSource;
    uint16_t minDest;
    uint16_t maxDest;
    int modbusChannel;

    MappingConfig() : index(0), moduleType(ModuleType::isAnalogicInputCurrent), minSource(0.0f), maxSource(0.0f), 
                      minDest(0), maxDest(0), modbusChannel(0) {}
};

#endif