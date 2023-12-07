#include "baseReader.h"
#include "../TCP Command server/CrioDebugServer.h"

//------- Creator --------

BaseReader::BaseReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance, std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
{
     //thread safe atomic variables
    m_sysConfig = aSysConfigInstance; //the object to handle Ni configuration via NiSysConfig API
    m_daqMx     = aDaqMxInstance;     //the object that handles IO operations withs the devices (crio and its modules)
    m_daqMx->channelCurrentDataReadySignal = std::bind(&BaseReader::onChannelDataReady, //Signal to Slot c++ style
                                              this,
                                              std::placeholders::_1,
                                              std::placeholders::_2);  //<-in that case 2 parameteres    
}

//---------- destructor -------------

BaseReader::~BaseReader()
{
}

//--------- ui ----------


void BaseReader::selectModuleAndChannel(const std::string &moduleName, const std::string &channelName)
{
    // Copy moduleName into m_manuallySelectedModuleName, ensuring no buffer overflow
    std::strncpy(m_manuallySelectedModuleName, moduleName.c_str(), sizeof(m_manuallySelectedModuleName) - 1);
    m_manuallySelectedModuleName[sizeof(m_manuallySelectedModuleName) - 1] = '\0';  // Null-terminate

    // Copy channelName into m_manuallySelectedChanName, ensuring no buffer overflow
    std::strncpy(m_manuallySelectedChanName, channelName.c_str(), sizeof(m_manuallySelectedChanName) - 1);
    m_manuallySelectedChanName[sizeof(m_manuallySelectedChanName) - 1] = '\0';  // Null-terminate
}


//--------- public slots ----------


void BaseReader::onChannelDataReady(double lastValue, QNiDaqWrapper *sender)
{
    onOneShotValueReaded(lastValue);
}

void BaseReader::onOneShotValueReaded(double aValue)
{
   if (aValue==std::numeric_limits<double>::min())
   {
      CrioDebugServer::broadcastMessage("Err:Read Error: value not valid");
   }
}


//----------- getters ---------------

std::shared_ptr<QNiSysConfigWrapper> BaseReader::getSysConfig() const
{
    return m_sysConfig;
}

std::shared_ptr<QNiDaqWrapper> BaseReader::getDaqMx() const
{
    return m_daqMx;
}

//-------------- setters ----------

void BaseReader::setSysConfig(const std::shared_ptr<QNiSysConfigWrapper> &newSysConfig)
{
    m_sysConfig = newSysConfig;
    if (sysConfigChangedSignal) 
    {
        sysConfigChangedSignal(m_sysConfig, this);
    }
}

void BaseReader::setDaqMx(const std::shared_ptr<QNiDaqWrapper> &newDaqMx)
{
    m_daqMx = newDaqMx;
    if (daqMxChangedSignal) 
    {
        daqMxChangedSignal(m_daqMx, this);
    }
}
