#include "NItoModbusBridge.h"
#include <cmath> // for std::sin
#include <cstdlib> // for std::rand
#include <vector>


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
    m_keyboardPoller = std::make_shared<KeyboardPoller>();         //this object handle keyboards events while polling (e.g. escape key to stop polling)    
    m_keyboardPoller->keyboardHitSignal = std::bind(&NItoModbusBridge::onKeyboardHit,this,std::placeholders::_1); //signal to slot
    
    
    m_simulateTimer = std::make_shared<SimpleTimer>();
    std::chrono::milliseconds mss(250);
    m_simulateTimer->setInterval(mss);
    m_simulateTimer->stop();
    // Wire up the signals and slots
    m_simulateTimer->setSlotFunction([this](){this->onSimulationTimerTimeOut();});


    m_dataAcquTimer = std::make_shared<SimpleTimer>();
    std::chrono::milliseconds msr(500);
    m_dataAcquTimer->setInterval(msr);
    m_dataAcquTimer->stop();
    // Wire up the signals and slots
    m_dataAcquTimer->setSlotFunction([this](){this->onDataAcquisitionTimerTimeOut();});

    this->newSimulationBufferReadySignal = [this](){m_modbusServer->updateSimulatedModbusAnalogRegisters(this);
                                                    /*showAnalogGridOnScreen(true);*/};

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
    if (!file.is_open()) {
        std::cerr << "Failed to open mapping.csv file" << std::endl;
        return;
    }

    std::string line;
    getline(file, line); // Skip the header line

    while (getline(file, line)) {
        std::istringstream iss(line);
        MappingConfig config;
        std::string token;

        getline(iss, token, ';'); config.index = std::stoi(token);
        getline(iss, token, ';'); config.moduleType = static_cast<ModuleType>(std::stoi(token));
        getline(iss, token, ';'); config.module = token;
        getline(iss, token, ';'); config.channel = token;
        getline(iss, token, ';'); config.minSource = std::stof(token);
        getline(iss, token, ';'); config.maxSource = std::stof(token);
        getline(iss, token, ';'); config.minDest = static_cast<uint16_t>(std::stoi(token));
        getline(iss, token, ';'); config.maxDest = static_cast<uint16_t>(std::stoi(token));
        getline(iss, token, ';'); config.modbusChannel = std::stoi(token);

        m_mappingData.push_back(config);
    }
}

void NItoModbusBridge::dataSynchronization() {
   
}

void NItoModbusBridge::startModbusSimulation()
{
    if (m_simulateTimer->isActive()) return;
    m_dataAcquTimer->stop();
    m_simulationBuffer.clear();
    m_keyboardPoller->start();
   // if (!m_modbusServer->isModbusActive())
  //  {
        m_modbusServer->run();
        std::cout<<"modbus part is running"<<std::endl;
  //  }
    m_simulateTimer->start();
}

void NItoModbusBridge::stopModbusSimulation()
{
    m_simulateTimer->stop();
    if (m_modbusServer->isModbusActive())
    {
        m_modbusServer->stop();
    }
}

void NItoModbusBridge::startAcquisition()
{
    if (m_dataAcquTimer->isActive()) return;
    m_simulateTimer->stop();
    m_dataAcquTimer->start();
}

void NItoModbusBridge::stopAcquisition()
{
    m_dataAcquTimer->stop();
}

uint16_t NItoModbusBridge::linearInterpolation16Bits(double value, double minSource, double maxSource, uint16_t minDestination, uint16_t maxDestination)
{
    // Calculate the scaling factor
    double scale = (maxDestination - minDestination) / (maxSource - minSource);
    // Perform the linear interpolation
    double mappedValue = minDestination + scale * (value - minSource);
    // Clamp the value within the destination boundaries and cast to uint16_t
    return static_cast<uint16_t>(std::min(std::max(mappedValue, static_cast<double>(minDestination)), static_cast<double>(maxDestination)));
}


void NItoModbusBridge::onKeyboardHit(char key)
{
    if (key==27)
    {
        if (m_simulateTimer->isActive())
        {
            m_simulateTimer->stop();
            m_simulationBuffer.clear();
        }
    }  
}

void NItoModbusBridge::onSimulationTimerTimeOut()
{
    constexpr uint64_t maxCounterValue = 18446744073709551615ULL;  // Max counter value for wrap-around;
    // Vector to hold simulated data for each channel
    std::vector<uint16_t> analogChannelsResult;
    simulateAnalogicInputs(analogChannelsResult);
    simulateCounters(analogChannelsResult);
    simulateCoders(analogChannelsResult);
    //simulateCounters(analogChannelsResult);
    // Push the new simulated data into the buffer
    m_simulationBuffer.clear();
    m_simulationBuffer.push_back(analogChannelsResult);     
    // Trigger a signal indicating that new simulated data is available, if the signal is set
    if (newSimulationBufferReadySignal)
    {
        newSimulationBufferReadySignal(); 
    }
    // Update the simulation counter and wrap around if necessary
    m_simulationCounter = (m_simulationCounter + 1) % maxCounterValue;
}

void NItoModbusBridge::simulateAnalogicInputs(std::vector<uint16_t> &analogChannelsResult)
{
     // Constants for simulating sine wave data
    constexpr double amplitude   = 50.0;                           // Amplitude of the sine wave
    constexpr double offset      = 50.0;                           // DC offset to shift the sine wave
    constexpr double omega       = 2.0 * M_PI / 1000.0;            // Frequency component for sine wave
    int              numChannels = m_modbusServer->nbSRUAnalogsIn(); // Number of channels to simulate
    

    // Calculate the sine value for the current simulation counter
    double sineValue = amplitude * std::sin(omega * m_simulationCounter) + offset;
    // Loop through each channel to generate simulated data
    if (m_modbusServer->modeSRU()) analogChannelsResult.push_back(static_cast<uint16_t>(0)); //to emulate the pnf config "particularity of 16 bit shift" we simply add an empty 16 bit unisgned integer
    for (int i = 0; i < numChannels; ++i)
    {
        // Generate random noise in the range of -0.1 to 0.1
        double noise = ((std::rand() % 21) - 10) / 100.0;
        // Add noise to the sine value
        double noisySineValue = sineValue * (1.0 + noise);
        // Map the noisy sine value to a 16-bit unsigned integer
        uint16_t mappedValue = linearInterpolation16Bits(noisySineValue, 0.0, 100.0, 0, 65535);
        // Add the mapped value to the result vector
        analogChannelsResult.push_back(mappedValue);
    }
}


void NItoModbusBridge::simulateCounters(std::vector<uint16_t> &analogChannelsResult)
{   
    for (int i = 0; i<m_modbusServer->nbSRUCounters();++i)
    {
        analogChannelsResult.push_back(32768); //first 16 bits are for frequency (spm) 0=0 ----> 65535=3000 for SRU
        m_simulatedCounterValue ++;
        uint16_t highValue = static_cast<uint16_t>((m_simulatedCounterValue >> 16) & 0xFFFF); // High 16 bits
        uint16_t lowValue = static_cast<uint16_t>  (m_simulatedCounterValue & 0xFFFF);        // Low 16 bits
        analogChannelsResult.push_back(highValue);    //16 bits for high (int 32)  
        analogChannelsResult.push_back(lowValue);    //16 bits for low (int 32)  
    }
}

void NItoModbusBridge::simulateCoders(std::vector<uint16_t> &analogChannelsResult)
{   
    for (int i = 0; i<m_modbusServer->nbSRUCoders();++i)
    {
        if (m_simulationCounter%4 == 0) m_simulatedCodersValue++;
        uint16_t highValue = static_cast<uint16_t>((m_simulatedCodersValue >> 16) & 0xFFFF); // High 16 bits
        uint16_t lowValue = static_cast<uint16_t>  (m_simulatedCodersValue & 0xFFFF);        // Low 16 bits       
        analogChannelsResult.push_back(highValue);    //16 bits for high (int 32)  
        analogChannelsResult.push_back(lowValue);    //16 bits for low (int 32)  
    }
}


uint16_t NItoModbusBridge::acquireData(const MappingConfig& config) {
    double readValue = 0.0;

    // Select the appropriate reader based on the module type
    std::shared_ptr<BaseReader> reader;
    switch (config.moduleType) {
        case ModuleType::ANALOG:
            reader = m_analogicReader;
            break;
        case ModuleType::DIGITAL:
            reader = m_digitalReader;
            break;
        // Handle other types (COUNTER, CODER) here or use a default case
        default:
            // Handle unexpected module types or throw an exception
            break;
    }

    if (reader) {
        reader->selectModuleAndChannel(config.module, config.channel);
        reader->manualReadOneShot(readValue);
    }

    return linearInterpolation16Bits(readValue, config.minSource, config.maxSource, config.minDest, config.maxDest);
}


void NItoModbusBridge::updateModbusRegisters() 
{
    for (const auto& config : m_mappingData) 
    {
        switch (config.moduleType) 
        {
            case ModuleType::ANALOG: 
            {
                uint16_t mappedValue = acquireData(config);
                m_modbusServer->setInputRegisterValue(config.modbusChannel, mappedValue);
                break;
            }
            case ModuleType::DIGITAL:
                // Implementation for digital modules
                break;
            // Handle other types (COUNTER, CODER) similarly
            default:
                // Handle unexpected module types or throw an exception
                break;
        }
    }
}


void NItoModbusBridge::onDataAcquisitionTimerTimeOut()
{
    //TODO
}

void NItoModbusBridge::showAnalogGridOnScreen(bool isSimulated)
{
    clearConsole();  // Ensure this function is properly declared and defined
    std::cout<<"press esc to stop simulation"<<std::endl;
    std::unique_ptr<StringGrid> resultGrid = std::make_unique<StringGrid>();
    // Declare a variable to hold the popped value
    std::vector<uint16_t> values;
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
        std::string csvString;
        // Iterate over the popped values
        for (size_t i = 1; i < values.size()+1; ++i) 
        {
            // Convert the uint16_t value to string
            std::string cellValue = std::to_string(values[i-1]);
            csvString += cellValue;
            (i%8 != 0) ? csvString += ";" : csvString += "\n";
        }
        //display
        resultGrid->setCSVFromString(csvString);
    } 
    else 
    {
        // Handle the case where the buffer was empty or an error occurred
        // Implement your logic here, like logging or displaying an error message
    }
}



// thread safe function to get the latest simulated data.
std::vector<uint16_t> NItoModbusBridge::getLatestSimulatedData() 
{
    std::vector<std::vector<uint16_t>> allData = m_simulationBuffer.copy();
    if (!allData.empty()) {
        return allData.back(); // Return the latest data.
    } else {
        return std::vector<uint16_t>(); // Return an empty vector if no data.
    }
}

ThreadSafeCircularBuffer<std::vector<uint16_t>>& NItoModbusBridge::getSimulationBuffer() {
    return m_simulationBuffer;
}

// Getter for m_simulateTimer
std::shared_ptr<SimpleTimer> NItoModbusBridge::getSimulateTimer() const 
{
    return m_simulateTimer;
}

// Getter for m_dataAcquTimer
std::shared_ptr<SimpleTimer> NItoModbusBridge::getDataAcquTimer() const 
{
    return m_dataAcquTimer;
}

// Getter for m_modbusServer
std::shared_ptr<ModbusServer> NItoModbusBridge::getModbusServer() const 
{
    return m_modbusServer;
}

// Getter for m_keyboardPoller
std::shared_ptr<KeyboardPoller> NItoModbusBridge::getKeyboardPoller() const 
{
    return m_keyboardPoller;
}

// Getter for m_mappingData
const std::vector<MappingConfig>& NItoModbusBridge::getMappingData() const 
{
    return m_mappingData;
}


