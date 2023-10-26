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
    analogChannelsResult.push_back(32768); //first 16 bits are for frequency
    
    analogChannelsResult.push_back(0);     //second 16 bits for high (int 32)
    analogChannelsResult.push_back(50);    //third 16 bits for low (int 32)  

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
    int              numChannels = m_modbusServer->nbSRUAnalogs(); // Number of channels to simulate
    

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
    if (m_simulatedCounterValues.size() < static_cast<std::size_t>(m_modbusServer->nbSRUCounters()))
    {
        m_simulatedCounterValues.reserve(m_modbusServer->nbSRUCounters()*2); //allocate twice the size we need ... why... well it's a scientific "large enough" approach
    }
    if (m_simulatedCounterValues.back() >= 4000000000) m_simulatedCounterValues[0] = 0; // Wrap around at a huge value (we do not care it's simulation)
    int startSlot = m_modbusServer->nbSRUAnalogs();
    startSlot++; //the first counter slot is just behind the last analog slot
    for (int i = 0; i < m_modbusServer->nbSRUCounters(); ++i)
    {
        m_simulatedCounterValues[i]++  ;   
        m_simulatedCounterValues[i]+=i ; //offset it to have different values on counters
        uint16_t highValue = static_cast<uint16_t>((m_simulatedCounterValues[i] >> 16) & 0xFFFF); // High 16 bits
        uint16_t lowValue = static_cast<uint16_t>  (m_simulatedCounterValues[i] & 0xFFFF);        // Low 16 bits
        //let's say spm is 1500 + counter index for the simulation
        uint16_t spm = 1500+i;
        analogChannelsResult[startSlot]      = lowValue;   //register 1 65
        analogChannelsResult[startSlot+1]    = highValue;  //register 2 66
        analogChannelsResult[startSlot+2]    = spm;        //register 3 67
        startSlot = startSlot + 3;                         //65 + 3 = 68 ----> next counter  
    }
    


    /*if (static_cast<int>(m_simulatedCounterValues.size()) < m_modbusServer->nbSRUCounters())
    {
        m_simulatedCounterValues.resize(m_modbusServer->nbSRUCounters());
    }
    
  
    m_simulatedCounterValues[0] = 2882400170; //++;
    for (int i = 1; i < m_modbusServer->nbSRUCounters(); ++i)
    {
        m_simulatedCounterValues[i] = 2882400170; //m_simulatedCounterValues[i - 1] + 1;
    }
    // Iterate through m_simulatedCounterValues and split each 32-bit value into two 16-bit values
    //Remove last element from vector to be aligned
    for (const auto& value : m_simulatedCounterValues)
    {
        uint32_t bigEndianValue = ((value & 0xFF000000) >> 24) |
                                  ((value & 0x00FF0000) >> 8)  |
                                  ((value & 0x0000FF00) << 8)  |
                                  ((value & 0x000000FF) << 24);
        
        // Split the 32-bit value into high and low 16-bit values
        uint16_t highValue = static_cast<uint16_t>((bigEndianValue >> 16) & 0xFFFF); // High 16 bits
        uint16_t lowValue = static_cast<uint16_t>(bigEndianValue & 0xFFFF);         // Low 16 bits
        if (m_modbusServer->modeSRU()) 
        // Push the high and low 16-bit values into the channel vector
        
       analogChannelsResult.push_back(0xFFFF);   
       analogChannelsResult.push_back(0xFFFF);
       analogChannelsResult.push_back(0xFFFF);
       analogChannelsResult.push_back(0xFFFF);
       analogChannelsResult.push_back(0xFFFF);
       analogChannelsResult.push_back(0xFFFF);
       
    }

    // Now, registersVector is filled with the 16-bit high and low values of each 32-bit value in m_simulatedCounterValues*/
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


