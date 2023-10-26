#ifndef NITOMODBUSBRIDGE_H
#define NITOMODBUSBRIDGE_H

#include <memory>
#include <functional>

#include "../channelReaders/analogicReader.h"
#include "../channelReaders/digitalReader.h"
#include "../channelWriters/digitalWriter.h"
#include "../Modbus/modbusServer.h"
#include "../globals/globalEnumStructs.h"
#include "../timers/simpleTimer.h"
#include "../circularBuffer/ThreadSafeCircularBuffer.h"
#include "../stringUtils/stringGrid.h"
#include "../stringUtils/stringUtils.h"
#include "../Signals/KeyboardPoller.h"
#include <algorithm> 



class NItoModbusBridge {
public:
    // Constructor
    NItoModbusBridge(std::shared_ptr<AnalogicReader> analogicReader,
                     std::shared_ptr<DigitalReader>  digitalReader,
                     std::shared_ptr<DigitalWriter>  digitalWriter,
                     std::shared_ptr<ModbusServer>   modbusServer);

    // Getters and setters for AnalogicReader
    std::shared_ptr<AnalogicReader> getAnalogicReader() const;
    void setAnalogicReader(std::shared_ptr<AnalogicReader> analogicReader);

    // Getters and setters for DigitalReader
    std::shared_ptr<DigitalReader> getDigitalReader() const;
    void setDigitalReader(std::shared_ptr<DigitalReader> digitalReader);

    // Getters and setters for DigitalWriter
    std::shared_ptr<DigitalWriter> getDigitalWriter() const;
    void setDigitalWriter(std::shared_ptr<DigitalWriter> digitalWriter);

    // TODO: Load mapping from a configuration file
    void loadMapping();
     // Data synchronization method
    void dataSynchronization();

    void startModbusSimulation();
    void stopModbusSimulation();
    void startAcquisition();
    void stopAcquisition();
    std::vector<uint16_t> getLatestSimulatedData(); 

private:
    unsigned long long m_simulationCounter=0;
    ThreadSafeCircularBuffer<std::vector<uint16_t>>     m_simulationBuffer;
    ThreadSafeCircularBuffer<std::vector<uint16_t>>     m_realDataBuffer;         
    std::shared_ptr<SimpleTimer>                         m_simulateTimer;
    std::shared_ptr<SimpleTimer>                         m_dataAcquTimer;
    std::shared_ptr<AnalogicReader>                      m_analogicReader;
    std::shared_ptr<DigitalReader>                       m_digitalReader;
    std::shared_ptr<DigitalWriter>                       m_digitalWriter;
    std::shared_ptr<ModbusServer>                        m_modbusServer;
    std::shared_ptr<KeyboardPoller>                      m_keyboardPoller;
    std::vector<MappingData>                             m_mappingData;

    uint16_t linearInterpolation16Bits(double value, double minSource, double maxSource, uint16_t minDestination, uint16_t maxDestination);

    void onKeyboardHit            (char key);
    void onSimulationTimerTimeOut ();
    void simulateAnalogicInputs   (std::vector<uint16_t> &analogChannelsResult);
    void simulateCounters         (std::vector<uint16_t> &analogChannelsResult);
    
    void onDataAcquisitionTimerTimeOut();
    void showAnalogGridOnScreen(bool isSimulated);

    // Signal functions to notify changes
    std::function<void()> onAnalogicReaderChanged;
    std::function<void()> onDigitalReaderChanged;
    std::function<void()> onDigitalWriterChanged;
    std::function<void()> newSimulationBufferReadySignal;

    std::vector<uint32_t> m_simulatedCounterValues;
};

#endif // NITOMODBUSBRIDGE_H
