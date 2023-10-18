#include "NItoModbusBridge.h"

// Constructor
NItoModbusBridge::NItoModbusBridge(std::shared_ptr<AnalogicReader> analogicReader,
                                   std::shared_ptr<DigitalReader> digitalReader,
                                   std::shared_ptr<DigitalWriter> digitalWriter,
                                   std::shared_ptr<ModbusServer>   modbusServer)
    : m_analogicReader(analogicReader),
      m_digitalReader(digitalReader),
      m_digitalWriter(digitalWriter),
      m_modbusServer(modbusServer)
{
    // Initialization can be done here if needed
}

// Getters and setters for AnalogicReader
std::shared_ptr<AnalogicReader> NItoModbusBridge::getAnalogicReader() const {
    return m_analogicReader;
}

void NItoModbusBridge::setAnalogicReader(std::shared_ptr<AnalogicReader> analogicReader) {
    m_analogicReader = analogicReader;
    if (onAnalogicReaderChanged) {
        onAnalogicReaderChanged();
    }
}

// Getters and setters for DigitalReader
std::shared_ptr<DigitalReader> NItoModbusBridge::getDigitalReader() const {
    return m_digitalReader;
}

void NItoModbusBridge::setDigitalReader(std::shared_ptr<DigitalReader> digitalReader) {
    m_digitalReader = digitalReader;
    if (onDigitalReaderChanged) {
        onDigitalReaderChanged();
    }
}

// Getters and setters for DigitalWriter
std::shared_ptr<DigitalWriter> NItoModbusBridge::getDigitalWriter() const {
    return m_digitalWriter;
}

void NItoModbusBridge::setDigitalWriter(std::shared_ptr<DigitalWriter> digitalWriter) {
    m_digitalWriter = digitalWriter;
    if (onDigitalWriterChanged) {
        onDigitalWriterChanged();
    }
}

void NItoModbusBridge::loadMapping() {
    std::ifstream file("mapping.csv");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        MappingData data;
        std::getline(iss, token, ';'); data.index = std::stoi(token);
        std::getline(iss, token, ';'); data.moduleIo = static_cast<ModuleIo>(std::stoi(token));
        std::getline(iss, token, ';'); data.moduleType = static_cast<ModuleType>(std::stoi(token));
        std::getline(iss, token, ';'); data.moduleSource = token;
        std::getline(iss, token, ';'); data.channelSource = token;
        std::getline(iss, token, ';'); data.minSource = std::stof(token);
        std::getline(iss, token, ';'); data.maxSource = std::stof(token);
        std::getline(iss, token, ';'); data.minDestination = std::stoi(token);
        std::getline(iss, token, ';'); data.maxDestination = std::stoi(token);
        std::getline(iss, token, ';'); data.destinationModbusChannel = std::stoi(token);
        m_mappingData.push_back(data);
    }
}

void NItoModbusBridge::dataSynchronization() {
    // Initialization
    // (Add any initialization code here if needed)

    // Timed Loop
 /*   while (true) {  // Replace with a proper loop exit condition
        // Read Data
        auto analogicData = m_analogicReader->readData();
        auto digitalData = m_digitalReader->readData();

        // Map Data
        for (const auto& mapping : m_mappingData) {
            // Perform linear interpolation for analogic data
            if (mapping.moduleType == ModuleType::Analog) {
                double rawValue = analogicData[mapping.channelSource];
                u_int16_t modbusValue = linearInterpolation16Bits(rawValue, mapping.minSource, mapping.maxSource, mapping.minDestination, mapping.maxDestination);
                // Store modbusValue or send it to Modbus layer
            }
            // Handle digital data
            else if (mapping.moduleType == ModuleType::Digital) {
                // Map digitalData to Modbus layer
            }
        }

        // Write Data
        // (Use m_digitalWriter to write mapped data to Modbus layer)

        // Sleep to control the loop frequency
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Sleep for 100 milliseconds

        // (Add any loop exit condition here)
    }

    // Cleanup
    // (Add any cleanup code here if needed)*/
}


u_int16_t NItoModbusBridge::linearInterpolation16Bits(double value, double minSource, double maxSource, u_int16_t minDestination, u_int16_t maxDestination)
{
    // Calculate the scaling factor
    double scale = (maxDestination - minDestination) / (maxSource - minSource);
    // Perform the linear interpolation
    double mappedValue = minDestination + scale * (value - minSource);
    // Clamp the value within the destination boundaries and cast to u_int16_t
    return static_cast<u_int16_t>(std::min(std::max(mappedValue, static_cast<double>(minDestination)), static_cast<double>(maxDestination)));
}
