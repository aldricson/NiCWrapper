#include "NItoModbusBridge.h"

// Constructor
NItoModbusBridge::NItoModbusBridge(std::shared_ptr<AnalogicReader> analogicReader,
                                   std::shared_ptr<DigitalReader> digitalReader,
                                   std::shared_ptr<DigitalWriter> digitalWriter,
                                   std::shared_ptr<ModbusServer>   modbusServer)
    : m_simulationBuffer(),
      m_realDataBuffer(), 
      m_analogicReader(analogicReader),
      m_digitalReader(digitalReader),
      m_digitalWriter(digitalWriter),
      m_modbusServer(modbusServer) // Initialize with size 20
{
    m_simulateTimer = std::make_shared<SimpleTimer>();
    std::chrono::milliseconds ms(250);
    m_simulateTimer->setInterval(ms);
    m_simulateTimer->stop();
    // Wire up the signals and slots
    m_simulateTimer->setSlotFunction([this](){this->onSimulationTimerTimeOut();});
    this->newSimulationBufferReadySignal = [this](){m_modbusServer->updateSimulatedModbusAnalogRegisters(this);
                                                    showAnalogGridOnScreen(true);};
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

void NItoModbusBridge::simulateModBusDatas()
{
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

#include <cmath> // for std::sin
#include <cstdlib> // for std::rand
#include <vector>

void NItoModbusBridge::onSimulationTimerTimeOut()
{
    // Constants for simulating sine wave data
    constexpr double amplitude = 50.0;  // Amplitude of the sine wave
    constexpr double offset = 50.0;     // DC offset to shift the sine wave
    constexpr double omega = 2.0 * M_PI / 1000.0;  // Frequency component for sine wave
    constexpr int numChannels = 64;     // Number of channels to simulate
    constexpr uint64_t maxCounterValue = 18446744073709551615ULL;  // Max counter value for wrap-around
    // Vector to hold simulated data for each channel
    std::vector<u_int16_t> analogChannelsResult;
    // Calculate the sine value for the current simulation counter
    double sineValue = amplitude * std::sin(omega * m_simulationCounter) + offset;
    // Loop through each channel to generate simulated data
    for (int i = 0; i < numChannels; ++i)
    {
        // Generate random noise in the range of -0.1 to 0.1
        double noise = ((std::rand() % 21) - 10) / 100.0;
        // Add noise to the sine value
        double noisySineValue = sineValue * (1.0 + noise);
        // Map the noisy sine value to a 16-bit unsigned integer
        u_int16_t mappedValue = linearInterpolation16Bits(noisySineValue, 0.0, 100.0, 0, 65535);
        // Add the mapped value to the result vector
        analogChannelsResult.push_back(mappedValue);
    }
    // Push the simulated data into the buffer
    m_simulationBuffer.push_back(analogChannelsResult);
    // Trigger a signal indicating that new simulated data is available, if the signal is set
    if (newSimulationBufferReadySignal)
    {
        newSimulationBufferReadySignal(); 
    }
    // Update the simulation counter and wrap around if necessary
    m_simulationCounter = (m_simulationCounter + 1) % maxCounterValue;
}



void NItoModbusBridge::showAnalogGridOnScreen(bool isSimulated)
{
    clearConsole();  // Ensure this function is properly declared and defined
    std::unique_ptr<StringGrid> resultGrid = std::make_unique<StringGrid>();
    // Declare a variable to hold the popped value
    std::vector<u_int16_t> values;
    // Initialize a boolean variable to check if pop_front succeeded
    bool popSuccess = false;
    if (isSimulated) {
        // Pop the front of the simulation buffer
        popSuccess = m_simulationBuffer.pop_front(values);
    } else {
        // Pop the front of the real data buffer
        popSuccess = m_realDataBuffer.pop_front(values);
    }
    // Check if pop_front succeeded
    if (popSuccess) {
        // Initialize a temporary vector to hold the strings for each row
        std::vector<std::string> tempRow;   
        // Iterate over the popped values
        for (size_t i = 0; i < values.size(); ++i) {
            // Convert the u_int16_t value to string
            std::string cellValue = std::to_string(values[i]);
            // Add the value to the temporary row
            tempRow.push_back(cellValue);
            // If we have collected 4 values or this is the last value, add a new row
            if (tempRow.size() == 4 || i == values.size() - 1) {
                resultGrid->addRow(tempRow);
                tempRow.clear();  // Clear the temporary row for the next set of values
            }
        }
        //display
        resultGrid->renderGrid();
    } 
    else 
    {
        // Handle the case where the buffer was empty or an error occurred
        // Implement your logic here, like logging or displaying an error message
    }
}



// thread safe function to get the latest simulated data.
std::vector<u_int16_t> NItoModbusBridge::getLatestSimulatedData() 
{
    std::vector<std::vector<u_int16_t>> allData = m_simulationBuffer.copy();
    if (!allData.empty()) {
        return allData.back(); // Return the latest data.
    } else {
        return std::vector<u_int16_t>(); // Return an empty vector if no data.
    }
}



