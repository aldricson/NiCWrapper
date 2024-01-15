#include "analogicReader.h"

AnalogicReader::AnalogicReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance, std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
    : BaseReader(aSysConfigInstance, aDaqMxInstance) // Call the base class constructor
{
    // Your AnalogReader constructor code here
}



void AnalogicReader::manualReadOneShot(const std::string &moduleAlias, const unsigned int &index, double &returnedValue)
{
    unsigned int channelIndex = index;
    NIDeviceModule *deviceModule     = m_sysConfig->getModuleByAlias(moduleAlias);

    ModuleType modType = deviceModule->getModuleType();  
    if (modType==isAnalogicInputCurrent)
    {
        double value;
        try
        {
            value = m_daqMx->readCurrent(deviceModule,channelIndex,50,true); 
            returnedValue = value;
        }
        catch(...)
        {
             onOneShotValueReaded(std::numeric_limits<double>::min());
             returnedValue = std::numeric_limits<double>::min();
        }    
        onOneShotValueReaded(value);
    }
    else if (modType==isAnalogicInputVoltage)
    {
        double value;
            try
            {
                value = m_daqMx->readVoltage(deviceModule,channelIndex,10);
                returnedValue = value;
            }
            catch(...)
            {
                onOneShotValueReaded(std::numeric_limits<double>::min());
                returnedValue = std::numeric_limits<double>::min();
            }
            onOneShotValueReaded(value);
    }
}

void AnalogicReader::manualReadOneShot(const std::string &moduleAlias, const std::string &chanName, double &returnedValue)
{
    NIDeviceModule *deviceModule     = m_sysConfig->getModuleByAlias(moduleAlias);

    ModuleType modType = deviceModule->getModuleType();  
    if (modType==isAnalogicInputCurrent)
    {
        double value;
        try
        {
            value = m_daqMx->readCurrent(deviceModule,chanName,50,true); 
            returnedValue = value;
        }
        catch(...)
        {
             onOneShotValueReaded(std::numeric_limits<double>::min());
             returnedValue = std::numeric_limits<double>::min();
        }    
        onOneShotValueReaded(value);
    }
    else if (modType==isAnalogicInputVoltage)
    {
        double value;
            try
            {
                value = m_daqMx->readVoltage(deviceModule,chanName,10);
                returnedValue = value;
            }
            catch(...)
            {
                onOneShotValueReaded(std::numeric_limits<double>::min());
                returnedValue = std::numeric_limits<double>::min();
            }
            onOneShotValueReaded(value);
    }
}
