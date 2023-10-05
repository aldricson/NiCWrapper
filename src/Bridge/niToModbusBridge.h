#ifndef NITOMODBUSBRIDGE_H
#define NITOMODBUSBRIDGE_H

#include <memory>
#include <functional>

#include "../channelReaders/analogicReader.h"
#include "../channelReaders/digitalReader.h"
#include "../channelWriters/digitalWriter.h"
#include "../globals/globalEnumStructs.h"
#include <algorithm> 



class NItoModbusBridge {
public:
    // Constructor
    NItoModbusBridge(std::shared_ptr<AnalogicReader> analogicReader,
                     std::shared_ptr<DigitalReader> digitalReader,
                     std::shared_ptr<DigitalWriter> digitalWriter);

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

private:
    std::shared_ptr<AnalogicReader> m_analogicReader;
    std::shared_ptr<DigitalReader> m_digitalReader;
    std::shared_ptr<DigitalWriter> m_digitalWriter;
    std::vector<MappingData> m_mappingData;

    u_int16_t linearInterpolation16Bits(double value, double minSource, double maxSource, u_int16_t minDestination, u_int16_t maxDestination);


    // Signal functions to notify changes
    std::function<void()> onAnalogicReaderChanged;
    std::function<void()> onDigitalReaderChanged;
    std::function<void()> onDigitalWriterChanged;
};

#endif // NITOMODBUSBRIDGE_H
