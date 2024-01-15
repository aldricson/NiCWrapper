#include "NItoModbusBridge.h"
#include <cmath>   // for std::sin
#include <cstdlib> // for std::rand
#include <vector>

// Constructor
NItoModbusBridge::NItoModbusBridge(std::shared_ptr<AnalogicReader> analogicReader,
                                   std::shared_ptr<DigitalReader> digitalReader,
                                   std::shared_ptr<NewModbusServer> modbusServer)
    : m_simulationBuffer(),
      m_realDataBuffer(),
      m_analogicReader(analogicReader),
      m_digitalReader(digitalReader),
      m_modbusServer(modbusServer)
{

    m_simulateTimer = std::make_shared<SimpleTimer>();
    std::chrono::milliseconds mss(250);
    m_simulateTimer->setInterval(mss);
    m_simulateTimer->stop();
    // Wire up the signals and slots
    m_simulateTimer->setSlotFunction([this]()
                                     { this->onSimulationTimerTimeOut(); });

    m_dataAcquTimer = std::make_shared<SimpleTimer>();
    std::chrono::milliseconds msr(500);
    m_dataAcquTimer->setInterval(msr);
    m_dataAcquTimer->stop();
    // Wire up the signals and slots
    m_dataAcquTimer->setSlotFunction([this]()
                                     { this->onDataAcquisitionTimerTimeOut(); });
}

// Getters and setters for AnalogicReader
std::shared_ptr<AnalogicReader> NItoModbusBridge::getAnalogicReader() const
{
    return m_analogicReader;
}

void NItoModbusBridge::setAnalogicReader(std::shared_ptr<AnalogicReader> analogicReader)
{
    m_analogicReader = analogicReader;
    if (onAnalogicReaderChanged)
    {
        onAnalogicReaderChanged();
    }
}

// Getters and setters for DigitalReader
std::shared_ptr<DigitalReader> NItoModbusBridge::getDigitalReader() const
{
    return m_digitalReader;
}

void NItoModbusBridge::setDigitalReader(std::shared_ptr<DigitalReader> digitalReader)
{
    m_digitalReader = digitalReader;
    if (onDigitalReaderChanged)
    {
        onDigitalReaderChanged();
    }
}

void NItoModbusBridge::loadMapping()
{

    std::ifstream file("mapping.csv");
    if (!file.is_open())
    {
        std::cerr << "Failed to open mapping.csv file" << std::endl;
        return;
    }

    std::string line;

    while (getline(file, line))
    {
        std::istringstream iss(line);
        MappingConfig config;
        std::string token;
        // tokenize each line
        getline(iss, token, ';');
        config.index = std::stoi(token);

        getline(iss, token, ';');
        config.moduleType = static_cast<ModuleType>(std::stoi(token));

        getline(iss, token, ';');
        config.module = token;

        getline(iss, token, ';');
        config.channel = token;

        getline(iss, token, ';');
        config.minSource = std::stof(token);

        getline(iss, token, ';');
        config.maxSource = std::stof(token);

        getline(iss, token, ';');
        config.minDest = static_cast<uint16_t>(std::stoi(token));

        getline(iss, token, ';');
        config.maxDest = static_cast<uint16_t>(std::stoi(token));

        getline(iss, token, ';');
        config.modbusChannel = std::stoi(token);

        m_mappingData.push_back(config);
    }
}


bool NItoModbusBridge::startModbusSimulation()
{
    if (m_simulateTimer->isActive())
        return true;
    try
    {
        m_dataAcquTimer->stop();
        m_simulationBuffer.clear();
        m_simulateTimer->start();
        return true;
    }
    catch (const std::exception &e)
    {
        // Handle the exception here (you can replace this comment with your actual error handling code)
        // You can also log the error message for debugging purposes.
        std::cerr << "An exception occurred: " << e.what() << std::endl;
        return false;
    }
}

void NItoModbusBridge::stopModbusSimulation()
{
    m_simulateTimer->stop();
}

bool NItoModbusBridge::startAcquisition()
{
    if (m_dataAcquTimer->isActive())
        return true;
    try
    {
        // clear the buffer
        m_realDataBuffer.clear();
        // calculate
        int bufferSize = m_modbusServer->getSRUMappingSizeWithoutAlarms();
        // clear the buffer line
        m_realDataBufferLine.clear();
        // resize and init it
        m_realDataBufferLine.resize(bufferSize, 0);

        m_simulateTimer->stop();
        m_dataAcquTimer->start();
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
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

void NItoModbusBridge::onSimulationTimerTimeOut()
{
    constexpr uint64_t maxCounterValue = 18446744073709551615ULL; // Max counter value for wrap-around;
    // Vector to hold simulated data for each channel
    std::vector<uint16_t> analogChannelsResult;
    simulateAnalogicInputs(analogChannelsResult);
    simulateCounters      (analogChannelsResult);
    simulateCoders        (analogChannelsResult);
    std::string str = "crio simulated values:\n";
    for (std::size_t i = 0; i < analogChannelsResult.size(); ++i)
    {
        str += std::to_string(analogChannelsResult[i]) + ";";
    }
    m_modbusServer->reMapInputRegisterValuesForAnalogics(analogChannelsResult);
    m_simulationCounter = (m_simulationCounter + 1) % maxCounterValue;
}

void NItoModbusBridge::simulateAnalogicInputs(std::vector<uint16_t> &analogChannelsResult)
{
    // Constants for simulating sine wave data
    constexpr double amplitude = 50.0;                                  // Amplitude of the sine wave
    constexpr double offset = 50.0;                                     // DC offset to shift the sine wave
    constexpr double omega = 2.0 * M_PI / 1000.0;                       // Frequency component for sine wave
    int numChannels = m_modbusServer->getSRUMapping().m_nbSRUAnalogsIn; // Number of channels to simulate
    bool isExlogCompatible = m_modbusServer->getSRUMapping().m_modeSRU; // To ensure compatibility with exlog
    // Calculate the sine value for the current simulation counter
    double sineValue = amplitude * std::sin(omega * m_simulationCounter) + offset;
    // Loop through each channel to generate simulated data
    if (isExlogCompatible)
        analogChannelsResult.push_back(static_cast<uint16_t>(0)); // to emulate the pnf config "particularity of 16 bit shift" we simply add an empty 16 bit unisgned integer
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
    for (int i = 0; i < m_modbusServer->getSRUMapping().m_nbSRUCounters; ++i)
    {
        analogChannelsResult.push_back(32768); // first 16 bits are for frequency (spm) 0=0 ----> 65535=3000 for SRU
        m_simulatedCounterValue++;
        uint16_t highValue = static_cast<uint16_t>((m_simulatedCounterValue >> 16) & 0xFFFF); // High 16 bits
        uint16_t lowValue  = static_cast<uint16_t>(m_simulatedCounterValue & 0xFFFF);          // Low 16 bits
        analogChannelsResult.push_back(highValue);                                            // 16 bits for high (int 32)
        analogChannelsResult.push_back(lowValue);                                             // 16 bits for low (int 32)
    }
}

void NItoModbusBridge::simulateCoders(std::vector<uint16_t> &analogChannelsResult)
{
    for (int i = 0; i < m_modbusServer->getSRUMapping().m_nbSRUCoders; ++i)
    {
        if (m_simulationCounter % 4 == 0)
            m_simulatedCodersValue++;
        uint16_t highValue = static_cast<uint16_t>((m_simulatedCodersValue >> 16) & 0xFFFF); // High 16 bits
        uint16_t lowValue = static_cast<uint16_t>(m_simulatedCodersValue & 0xFFFF);          // Low 16 bits
        analogChannelsResult.push_back(highValue);                                           // 16 bits for high (int 32)
        analogChannelsResult.push_back(lowValue);                                            // 16 bits for low (int 32)
    }
}

void NItoModbusBridge::acquireData()
{
    std::cout<<"enter acquire data"<<std::endl;
    for (std::size_t i = 0; i < m_mappingData.size(); ++i)
    {
        // remember it was loaded from the csv so let's get the structure representing each line of the csv
        MappingConfig lineCfg = m_mappingData[i];
        //initialization
        double result                = 0.0                   ; 
        double minInput              = lineCfg.minSource     ;
        double maxInput              = lineCfg.maxSource     ;
        uint16_t minOutput           = lineCfg.minDest       ;
        uint16_t maxOutput           = lineCfg.maxDest       ;
        int      destinationRegister = lineCfg.modbusChannel ;
        switch (lineCfg.moduleType)
        {
            case ModuleType::isAnalogicInputCurrent :
            case ModuleType::isAnalogicInputVoltage :
            {

                m_analogicReader->manualReadOneShot(lineCfg.module,lineCfg.channel,result);
                uint16_t interpolatedResult = linearInterpolation16Bits(result,minInput,maxInput,minOutput,maxOutput);
                m_realDataBufferLine[destinationRegister]=interpolatedResult;
                std::cout<<lineCfg.module.c_str()<<" "<<lineCfg.channel.c_str()<<" "<<interpolatedResult<<std::endl;
                break;
            }
            case ModuleType::isCoder :
            {
                break;
            }
            case ModuleType::isCounter :
            {
                break;
            }
            case ModuleType::isDigitalInput :
            {
                break;
            }
        }  
    } 
    //here our buffer line is ready we just have to remap it in the registers
    m_modbusServer->reMapInputRegisterValuesForAnalogics(m_realDataBufferLine);
}




void NItoModbusBridge::onDataAcquisitionTimerTimeOut()
{
    acquireData();
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
std::shared_ptr<NewModbusServer> NItoModbusBridge::getModbusServer() const 
{
    return m_modbusServer;
}


// Getter for m_mappingData
const std::vector<MappingConfig>& NItoModbusBridge::getMappingData() const 
{
    return m_mappingData;
}
