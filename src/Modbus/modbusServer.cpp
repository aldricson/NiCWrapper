#include "ModbusServer.h"
#include <string.h>
#include "../Bridge/niToModbusBridge.h"

/***************************************************************
 * @file       ModbusServer.cpp
 * @brief      Initializes the Modbus server
 * @param      Host_Ip: IP address of the host (default is "127.0.0.1")
 * @param      port: Port number (default is 502)
 * @param      debugging: Enable or disable debugging (default is true)
 * @version    v1
 * @return     true if initialization is successful, false otherwise
 * @date       2021/10/6
 **************************************************************/
bool ModbusServer::initModbus(std::string Host_Ip = "127.0.0.1", int port = 502, bool debugging = true)
{
    std::string str;
    str=drawCell(23,"create modbus context");
    std::cout<<str<<std::endl;
    // Create a new Modbus context for TCP/IPv4
    ctx = modbus_new_tcp(Host_Ip.c_str(), port);
    std::cout<<"[context]"<<std::endl<<"listening on "<<Host_Ip<<" port:"<<std::to_string(port)<<std::endl;
    std::cout<<"set modbus debug flag to "<<std::to_string(debugging)<<std::endl;
    // Enable or disable debugging based on the parameter
    modbus_set_debug(ctx, debugging);
    // Check if the Modbus context was successfully created
    if (ctx == NULL)
    {
        fprintf(stderr, "There was an error allocating the Modbus context\n");
        throw -1;
    }
    str=drawCell(31,"create modbus context SUCCESS");
    std::cout<<str<<std::endl;
    // Listen for incoming Modbus requests on the created context
    str = drawCell(34,"modbus tcp socket in listen mode");
    std::cout<<str<<std::endl;
    m_modbusSocket = modbus_tcp_listen(ctx, 1);
    fdmax = m_modbusSocket;
    // Create a new Modbus mapping with the specified number of bits and registers
    // The parameters are for coils, discrete inputs, input registers, and holding registers respectively
    mapping = modbus_mapping_new(m_nbCoils, m_numInputBits, m_numInputRegisters, m_numRegisters);
    // Check if the Modbus mapping was successfully created
    if (mapping == NULL)
    {
        fprintf(stderr, "Unable to assign mapping: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        m_initialized = false;
        return false;
    }
    // Mark the Modbus server as initialized
    str = drawCell(23,"modbus server is UP !");
    std::cout<<str<<std::endl;
    m_initialized = true;
    return true;
}


ModbusServer::ModbusServer(string host, uint16_t port)
{
    initModbus(host, port, false);
    m_stopRequested.store(false);
    m_threadRunning.store(false);
}

ModbusServer::~ModbusServer()
{
    modbus_mapping_free(mapping);
    modbus_close(ctx);
    modbus_free(ctx);
}


// Run the Modbus server in a separate thread to continuously receive messages
void ModbusServer::run() {
    // Check if the server is already running
    if (!m_stopRequested.load() && !m_threadRunning.load()) {
        std::cout << "Starting Modbus server thread..." << std::endl;

        // Set the thread running flag
        m_threadRunning.store(true);

        // Reset the stop requested flag
        m_stopRequested.store(false);

        // Create and detach the thread
        std::thread loop([this]() {
            std::cout << "Modbus server thread loop started." << std::endl;
            while (!m_stopRequested.load()) {
                // Existing loop content...
            }
            // Clear the thread running flag when the loop exits
            m_threadRunning.store(false);
            std::cout << "Modbus server thread loop exited." << std::endl;
        });
        loop.detach();
    } else {
        std::cout << "Modbus server is already running." << std::endl;
    }
}


// stop the Modbus server
void ModbusServer::stop()
{
     std::lock_guard<std::mutex> lock(slavemutex);
    m_stopRequested.store(true);  // Set the stop flag using store()
    // Disconnect all clients before shutting down the server
     // Disconnect all clients before shutting down the server
    for (int master_socket = 0; master_socket <= fdmax; master_socket++)
    {
        // Skip the server socket
        if (master_socket == m_modbusSocket)
        {
            continue;
        }
        // Close the client socket
        if (close(master_socket) == -1)
        {
            std::cout << "Error closing client socket" << std::endl;
        }
        else
        {
            printf("Successfully closed connection on socket %d\n", master_socket);
        }

        // Remove the socket from the set
        FD_CLR(master_socket, &refset);  // Using the member variable here
    }
    // Close the Modbus context and free resources
    modbus_mapping_free(mapping);
    modbus_close(ctx);
    modbus_free(ctx);
    m_initialized = false;  // Mark the server as uninitialized
}


// Function to check if the Modbus server is active
bool ModbusServer::isModbusActive()
{
    // Lock the mutex to ensure thread safety while checking the state
    slavemutex.lock();   
    // Check if the Modbus server is initialized and not stopped
    bool isActive = m_initialized && !m_stopRequested.load();
    // Unlock the mutex
    slavemutex.unlock();
    return isActive;
}

int ModbusServer::nbSRUAnalogsIn()
{
    return m_nbSRUAnalogsIn;
}

int ModbusServer::nbSRUCounters()
{
    return m_nbSRUCounters;
}

int ModbusServer::nbSRUCoders()
{
    return m_nbSRUCoders;
}

bool ModbusServer::modeSRU()
{
    return m_modeSRU;
}

// Set the Modbus slave ID for the server
bool ModbusServer::modbus_set_slave_id(int id)
{

    int clamped = id; 
    if (clamped < 0)
    {
        clamped = 0;
    }
    else if (clamped > 255)
    {
        clamped = 255;
    }
    // Call the libmodbus function to set the slave ID
    int rc = modbus_set_slave(ctx, clamped);
    m_slave_Id = static_cast<uint8_t>(clamped); 
    // Check if setting the slave ID was successful
    if (rc == -1)
    {
        // Print an error message and free the Modbus context
        fprintf(stderr, "Invalid slave id\n");
        modbus_free(ctx);
        return false;
    }
    
    return true;
}

// Set a floating-point value in the Modbus server's input register
bool ModbusServer::setInputRegisterValue(int registerStartaddress, float Value)
{
    // Check if the register address is within the valid range
    // Note: We check for (m_numRegisters - 2) because a float takes two registers
    if (registerStartaddress > (m_numRegisters - 2))
    {
        return false;
    }
    // Lock the mutex to ensure thread safety
    slavemutex.lock();
    // Use libmodbus function to set the float value in the input register
    // This function handles the conversion to Modbus format
    modbus_set_float(Value, &mapping->tab_input_registers[registerStartaddress]);
    
    // Unlock the mutex
    slavemutex.unlock();
    
    return true;
}

// Set a 16-bit value in the Modbus server's input register
bool ModbusServer::setInputRegisterValue(int registerNumber, uint16_t Value)
{
    // Check if the register address is within the valid range
    if (registerNumber > (m_numRegisters - 1))
    {
        return false;
    }
    // Set the value in the input register
    mapping->tab_input_registers[registerNumber] = Value;

    return true;
}



// Set a 32-bit value in the Modbus server's input register (for example for counters)
bool ModbusServer::setInputRegisterValue(int registerNumber, uint32_t value)
{
    // Check if the register address and the next one are within the valid range
    if (registerNumber > (m_numRegisters - 2))
    {
        return false;
    }

    // Split the 32-bit value into two 16-bit values
    uint16_t highValue = static_cast<uint16_t>((value >> 16) & 0xFFFF); // High 16 bits
    uint16_t lowValue = static_cast<uint16_t>(value & 0xFFFF);         // Low 16 bits

    // Set the high 16 bits in the first input register
    mapping->tab_input_registers[registerNumber] = highValue;

    // Set the low 16 bits in the next input register
    mapping->tab_input_registers[registerNumber + 1] = lowValue;

    return true;
}


// Initialize the Modbus server's holding register with a 16-bit unsigned integer value
bool ModbusServer::setHoldingRegisterValue(int registerNumber, uint16_t Value)
{
    // Check if the register address is within the valid range
    if (registerNumber > (m_numRegisters - 1))
    {
        return false;
    }
    // Lock the mutex to ensure thread safety
    slavemutex.lock();
    // Set the value in the holding register
    mapping->tab_registers[registerNumber] = Value;
    // Unlock the mutex
    slavemutex.unlock();
    return true;
}

// Retrieve the value from the Modbus server's holding register
uint16_t ModbusServer::getHoldingRegisterValue(int registerStartaddress)
{
    // Check if the Modbus server is initialized
    if (!m_initialized)
    {
        return -1;
    }
    // Return the value from the holding register
    return mapping->tab_registers[registerStartaddress];
}

// Set the value of a specific bit in the Modbus server's input register
bool ModbusServer::setTab_Input_Bits(int NumBit, uint8_t Value)
{
    // Check if the bit number is within the valid range
    if (NumBit > (m_numInputBits - 1))
    {
        return false;
    }
    // Lock the mutex to ensure thread safety
    slavemutex.lock();
    // Set the value of the specific bit in the input register
    mapping->tab_input_bits[NumBit] = Value;
    // Unlock the mutex
    slavemutex.unlock();
    return true;
}

// Retrieve the value of a specific bit from the Modbus server's input register
uint8_t ModbusServer::getTab_Input_Bits(int NumBit)
{
    // Check if the Modbus server is initialized
    if (!m_initialized)
    {
        return -1;
    }
    // Return the value of the specific bit from the input register
    return mapping->tab_input_bits[NumBit];
}

// Set a floating-point value in the Modbus server's holding register
bool ModbusServer::setHoldingRegisterValue(int registerStartaddress, float Value)
{
    // Check if the register address is within the valid range
    if (registerStartaddress > (m_numRegisters - 2))
    {
        return false;
    }
    // Lock the mutex to ensure thread safety
    slavemutex.lock();
    // Set the floating-point value in the holding register (in little-endian mode)
    modbus_set_float(Value, &mapping->tab_registers[registerStartaddress]);   
    // Unlock the mutex
    slavemutex.unlock();
    return true;
}



void ModbusServer::updateSimulatedModbusAnalogRegisters(NItoModbusBridge *bridge)
{
    std::vector<uint16_t> latestData = bridge->getLatestSimulatedData();
    std::size_t i = 0;
    for (const auto& data : latestData) 
    {
        setInputRegisterValue(i, data);
        ++i;
    }
}

// Get a floating-point value from the Modbus server's holding register
float ModbusServer::getHoldingRegisterFloatValue(int registerStartaddress)
{
    // Check if the Modbus server is initialized
    if (!m_initialized)
    {
        return -1.0f;
    }    
    // get the float value from the holding register
    // This function handles the conversion from Modbus format
    return unsignedIntPairToFloat(&mapping->tab_registers[registerStartaddress]);
}

void ModbusServer::showTCPFrameForDebugPurpose(const uint8_t *query, const uint8_t functionCode, const uint16_t startingAddres, const uint16_t quantity, const int rc)
{          
    // Debugging: Log the entire Modbus query frame
    // a modbusframe looks like 0 0 0 0 0 6 1 4 0 1 0 40 if osiris is asking
    // each number (or number pair) is a field 
    //[Transaction Identifier] [Protocol Identifier] [Length Field] [Unit Identifier] [Function Code] [Data]
    //For example, consider the frame 0 0 0 0 0 6 1 4 0 1 0 40:
    //Transaction Identifier: 0 0 (2 bytes)
    //Protocol Identifier: 0 0 (2 bytes, always 0 for Modbus)
    //Length Field: 0 6 (2 bytes, length of the remaining bytes)
    //Unit Identifier: 1 (1 byte, usually 1 for Modbus TCP)
    //Function Code: 4 (1 byte, Read Analog Input Registers)
    //Data: 0 1 0 40 (Start address and quantity of registers to read)
    std::cout << std::endl;
    std::cout << "Received Modbus query frame: ";
    for (int i = 0; i < rc; ++i)
    {
        std::cout << "0x" << std::hex << static_cast<int>(query[i]) << " ";
    }
    std::cout << std::endl;
    //Human-readable breakdown of the Modbus query frame
    std::cout << "*****************************" << std::endl;
    std::cout << "* Human-readable breakdown: *" << std::endl;
    std::cout << "*****************************" << std::endl;
    std::cout << std::endl;
    std::cout << "  Transaction Identifier: "  << "0x" << std::hex << static_cast<int>(query[0]) << " " << "0x" << static_cast<int>(query[1]) << std::endl;
    std::cout << "  Protocol Identifier: " << "0x" << std::hex << static_cast<int>(query[2]) << " " << "0x" << static_cast<int>(query[3]) << std::endl;
    std::cout << "  Length Field: " << "0x" << std::hex << static_cast<int>(query[4]) << " " << "0x" << static_cast<int>(query[5]) << std::endl;
    std::cout << "  Unit Identifier: " << "0x" << std::hex << static_cast<int>(query[6]) << std::endl;
    std::cout << "  Function Code: " << "0x" << std::hex << static_cast<int>(query[7]) << std::endl;
    if (query[7]==functionCode)
    {
        std::cout << "  Function verified ok: ";
        switch (functionCode)
        {
            case 0x04 :
            {
                std::cout<<"Read input registers" << endl;
                std::cout<<"  Registers start adress: "<< std::to_string(startingAddres) << endl;
                std::cout<<"  Asked quantity: "<< std::to_string(quantity);
            }
        }
    }
    std::cout << "  Data: ";
    for (int i = 8; i < rc; ++i)
    {
       std::cout << std::hex << static_cast<int>(query[i]) << " ";
    }
    std::cout << std::endl;
                    
}

bool ModbusServer::parseReceivedData(const uint8_t *buffer, int bytesRead, uint8_t &functionCode, uint16_t &startingAddress, uint16_t &quantity)
{
    // Check if the received data is at least the minimum length for a Modbus TCP frame
    if (bytesRead < 9) {
        std::cerr << "Received data is too short to be a valid Modbus TCP frame." << std::endl;
        return false;
    }
    // In a Modbus TCP frame, the function code is at the 8th byte (index 7)
    functionCode = buffer[7];
    // The starting address is at the 9th and 10th bytes (index 8 and 9)
    // Modbus uses big-endian, so we need to swap the bytes
    startingAddress = (buffer[8] << 8) | buffer[9];
    // The quantity is at the 11th and 12th bytes (index 10 and 11)
    // Again, Modbus uses big-endian
    quantity = (buffer[10] << 8) | buffer[11];
    return true;
}


//Function to calcultae remaining bytes to follow after transactionId and protocolId 
bool ModbusServer::calculateRemainingBytes(const int totalLengthInBytes,
                                           uint8_t &highByte,
                                           uint8_t &lowByte)
{
    // Step 1: Force totalLengthInBytes into the range [0, 65535]
    std::cout<<"total length :"<<std::to_string(totalLengthInBytes)<<" bytes"<<endl;
    int clampedLength = totalLengthInBytes-6; //-6 because we substract 2 octets for the transaction Id
                                              //                        2 octets for the protocol Id
                                              //                        2 octets for the length  
    if (clampedLength < 0)
    {
        clampedLength = 0;
    }
    else if (clampedLength > 65535)
    {
        clampedLength = 65535;
    }
    
    //the first one at this stage will be the device address (1 byte)
    //then comes the function code                           (1 byte)
    //then the payload size                                  (1 byte)
    //then the payload! So... 1+1+1=3 that's a minimum to have a payload size ok, But if there's no payload after then there's a problem
    //that's why we check it here
    if (clampedLength<=3)
    {
        std::cout<<"Protocol error the number of remaining bytes is too small for a valid Modbus frame"<<std::endl;
        return false;
    } 
    // Step 1b: Convert to uint16_t if needed (it's probably not obligatory BUT STILL it's good practice when you play with bytes to be very precise on typing variables)
    uint16_t clampedLengthUint16 = static_cast<uint16_t>(clampedLength);

    // Step 2: Split into high and low bytes
    highByte = (clampedLengthUint16 >> 8) & 0xFF; // Extract the high byte
    lowByte = clampedLengthUint16 & 0xFF;         // Extract the low byte

    return true; // Everything went right
}

// Validate the starting address and quantity for reading registers
bool ModbusServer::validateAddressAndQuantity(uint16_t startingAddress, uint16_t quantity, uint16_t maxRegisters)
{
    if (startingAddress >= maxRegisters || (startingAddress + quantity) > maxRegisters)
    {
        std::cerr << "Invalid starting address or quantity for reading registers." << std::endl;
        return false;
    }
    return true;
}

// Prepare the response header for Modbus TCP frame
void ModbusServer::prepareResponseHeader(
                                          uint8_t transactionIdHigh,
                                          uint8_t transactionIdLow,
                                          uint8_t protocolIdHigh,
                                          uint8_t protocolIdLow,
                                          uint8_t functionCode,
                                          uint16_t quantity,
                                          uint8_t *responseBuffer)
{
    uint8_t clampedQuantity;
    if (quantity<0)
    {
        quantity = 0;
    }
    else if (quantity>128)
    {
       quantity = 128;
    } 
    clampedQuantity = static_cast<uint8_t>(quantity);
    // Prepare the response header
    responseBuffer[0] = transactionIdHigh;            // Transaction Identifier (High byte)
    responseBuffer[1] = transactionIdLow;             // Transaction Identifier (Low byte)
    responseBuffer[2] = protocolIdHigh;               // Protocol Identifier (High byte)
    responseBuffer[3] = protocolIdLow;                // Protocol Identifier (Low byte)
    responseBuffer[4] = 0x00;                         // Length (High byte)
    responseBuffer[5] = 0x00;                         // Length (Low byte)
    responseBuffer[6] = m_slave_Id;                   // Unit Identifier
    responseBuffer[7] = functionCode;                 // Function Code for Read Input Registers
    responseBuffer[8] = clampedQuantity * sizeof(uint16_t); // Byte count of the payload *2 (uint_16t is 2 bytes, so size of will return 2) because we count in bytes but each values is 16 bits (2 bytes)
}

// Fill the response payload for Modbus TCP frame
int ModbusServer::fillResponsePayload(uint16_t *payload, uint16_t quantity, uint8_t *responseBuffer, int startIndex)
{
    int responseIndex = startIndex;
    for (uint16_t i = 0; i < quantity; ++i)
    {
        uint16_t registerValue = payload[i];
        responseBuffer[responseIndex++] = (registerValue >> 8) & 0xFF; // High byte
        responseBuffer[responseIndex++] = registerValue & 0xFF;        // Low byte
    }
    return responseIndex;
}

// Overloaded function to fill response payload for uint8_t
int ModbusServer::fillResponsePayload(uint8_t *payload, int payloadSize, uint8_t *responseBuffer, int startIndex)
{
    // Initialize the response index to the starting index
    int responseIndex = startIndex;
    // Loop through the payload and fill the response buffer
    for (int i = 0; i < payloadSize; ++i)
    {
        // Directly copy the 8-bit value from payload to responseBuffer
        responseBuffer[responseIndex++] = payload[i];
    }
    // Return the updated response index
    return responseIndex;
}


//function to read coils
int ModbusServer::readCoils(
    uint8_t transactionIdHigh, uint8_t transactionIdLow,
    uint8_t protocolIdHigh, uint8_t protocolIdLow,
    uint8_t functionCode, uint16_t startingAddress,
    uint16_t quantity, uint8_t *responseBuffer, modbus_t *ctx)
{
    // Validate the starting address and quantity against the total number of coils (m_numCoils)
    if (!validateAddressAndQuantity(startingAddress, quantity, m_nbCoils))
    {
        return -1;  // Return -1 if validation fails
    }

    // Calculate the number of bytes needed to represent the coils
    // Coils are packed 8 to a byte, so (quantity + 7) / 8 rounds up to the nearest byte
    int payloadSize = (quantity + 7) / 8;

    // Create a buffer to hold the coil values
    uint8_t *payload = new uint8_t[payloadSize];

    // Initialize all elements in the payload to zero
    memset(payload, 0, payloadSize);

    // Populate the payload buffer with coil values
    for (size_t i = 0; i < quantity; ++i)
    {
        // Check if the coil at the current address is set (1)
        if (mapping->tab_bits[startingAddress + i])
        {
            // Set the corresponding bit in the payload
            //i / 8: This calculates which byte in the payload array we are dealing with.
            //Since each byte can represent 8 coils, we divide the coil index i by 8 to find the byte index.
            //i % 8: This calculates which bit within that byte we are dealing with. The remainder of i / 8 gives us the bit position within the byte.
            //1 << (i % 8): This shifts the binary representation of 1 to the left by (i % 8) positions.
            //This effectively creates a bitmask where only the bit at the position (i % 8) is set to 1.
            //payload[i / 8] |= (1 << (i % 8)): This uses the bitwise OR assignment operator |= to set the bit at the position (i % 8) in payload[i / 8] to 1.
            //yeah right, Sidali once you master bytewise operations you can do all that in a single lightening fast line :
            payload[i / 8] |= (1 << (i % 8));
        }
    }
    // Prepare the response header with transaction ID, protocol ID, and other details
    prepareResponseHeader(transactionIdHigh, transactionIdLow, protocolIdHigh, protocolIdLow, functionCode, quantity, responseBuffer);
    // Set the byte count field in the response buffer
    // This is the number of bytes that the coils are packed into
    responseBuffer[8] = payloadSize;
    // Fill the response payload starting from index 9 in the response buffer
    int responseIndex = fillResponsePayload(payload, payloadSize, responseBuffer, 9);    
    // Declare variables to hold the high and low bytes of the total remaining length
    uint8_t totalRemainingLengthHighByte;
    uint8_t totalRemainingLengthLowByte;
    // Calculate the remaining bytes in the response frame
    if (!calculateRemainingBytes(responseIndex, totalRemainingLengthHighByte, totalRemainingLengthLowByte))
    {
        std::cout << "Impossible to calculate remaining bytes of the response frame" << std::endl;
        return 0;  // Return 0 if calculation fails
    }
    else
    {
        // Update the length field in the response header with the calculated high and low bytes
        responseBuffer[4] = totalRemainingLengthHighByte;  // Length (High byte)
        responseBuffer[5] = totalRemainingLengthLowByte;   // Length (Low byte)
    }
    // Free the dynamically allocated payload buffer
    delete[] payload;
    // Return the total length of the response
    return responseIndex;
}


int ModbusServer::writeSingleCoil(
    uint8_t transactionIdHigh, uint8_t transactionIdLow,
    uint8_t protocolIdHigh, uint8_t protocolIdLow,
    uint8_t functionCode, uint16_t coilAddress,
    bool value, uint8_t *responseBuffer, modbus_t *ctx)
{
    // Validate the coil address
    if (coilAddress >= m_nbCoils)
    {
        return -1;
    }
    // Update the coil value in the mapping
    mapping->tab_bits[coilAddress] = value;
    // Prepare the response header
    prepareResponseHeader(transactionIdHigh, transactionIdLow, protocolIdHigh, protocolIdLow, functionCode, 1, responseBuffer);
    // Fill the response payload with the coil address and value
    responseBuffer[8] = (coilAddress >> 8) & 0xFF;  // Coil address high byte
    responseBuffer[9] = coilAddress & 0xFF;         // Coil address low byte
    responseBuffer[10] = value ? 0xFF : 0x00;       // Coil value
    // Update the length field in the response header
    responseBuffer[4] = 0;  // Length (High byte)
    responseBuffer[5] = 6;  // Length (Low byte)
    return 11;  // Return the total length of the response
}



// Function to read input registers and prepare the response
int ModbusServer::readInputRegisters(
                                      uint8_t transactionIdHigh,
                                      uint8_t transcationIdLow,
                                      uint8_t protocolIdHigh,
                                      uint8_t protocolIdLow,
                                      uint8_t functionCode,
                                      uint16_t startingAddress,
                                      uint16_t quantity,
                                      uint8_t *responseBuffer,
                                      modbus_t *ctx)
{
    // Check if the starting address and quantity are within the valid range
    // Validate the starting address and quantity
    if (!validateAddressAndQuantity(startingAddress, quantity, m_numInputRegisters))
    {
        return -1;
    }
    // Create a buffer to hold the register values
    uint16_t *payload = new uint16_t[quantity];
    // Use memset to set all elements to zero
    memset(payload, 0, quantity * sizeof(uint16_t));

    for (size_t i = 0; i < quantity; ++i)
    {
        payload[i] = mapping->tab_input_registers[i]; //kind of get on the modbus registers
    }
    // Prepare the response header
    prepareResponseHeader(transactionIdHigh, transcationIdLow, protocolIdHigh, protocolIdLow, functionCode, quantity, responseBuffer); 
     // prepare the response payload
    int responseIndex = fillResponsePayload(payload, quantity, responseBuffer, 9);    
    //from here the payload is ready, the header is... well... not so ready yet, let's complete it
    // Update the length field in the response header
    uint8_t totalRemainingLengthHighByte; //let's prepare 2 x 8 bit regiters... because our length will be 16 bits 
    uint8_t totalRemainingLengtLowByte;   //and we need to cut it off in high and low value
    if (!calculateRemainingBytes(responseIndex, totalRemainingLengthHighByte, totalRemainingLengtLowByte))
    {
        std::cout << "Impossible to calculate remaining bytes of the response frame" << std::endl;
        return 0;
    }
    else
    {
        responseBuffer[4] = totalRemainingLengthHighByte; // Length (High byte)
        responseBuffer[5] = totalRemainingLengtLowByte;   // Length (Low byte)
    }
    // Free the resources
    delete[] payload;
    return responseIndex; // Return the total length of the response
}


// Function to read holding registers and prepare the response
// Function to read holding registers and prepare the response
int ModbusServer::readHoldingRegisters(
    uint8_t transactionIdHigh, uint8_t transactionIdLow,
    uint8_t protocolIdHigh, uint8_t protocolIdLow,
    uint8_t functionCode, uint16_t startingAddress,
    uint16_t quantity, uint8_t *responseBuffer, modbus_t *ctx)
{
    // Validate the starting address and quantity
    if (!validateAddressAndQuantity(startingAddress, quantity, m_numRegisters))
    {
        return -1;
    }

    // Lock the mutex to ensure thread safety
    std::lock_guard<std::mutex> lock(slavemutex);

    // Create a buffer to hold the register values
    uint16_t *payload = new uint16_t[quantity];
    memset(payload, 0, quantity * sizeof(uint16_t)); // Initialize all elements to zero

    // Populate the payload buffer with register values
    for (size_t i = 0; i < quantity; ++i)
    {
        payload[i] = mapping->tab_registers[startingAddress + i];
    }

    // Prepare the response header
    prepareResponseHeader(transactionIdHigh, transactionIdLow, protocolIdHigh, protocolIdLow, functionCode, quantity, responseBuffer);

    // Byte count of the payload
    responseBuffer[8] = quantity * 2; // Each register is 2 bytes

    // Fill the response payload
    int responseIndex = fillResponsePayload(payload, quantity, responseBuffer, 9);

    // Update the length field in the response header
    uint8_t totalRemainingLengthHighByte;
    uint8_t totalRemainingLengthLowByte;
    if (!calculateRemainingBytes(responseIndex, totalRemainingLengthHighByte, totalRemainingLengthLowByte))
    {
        std::cout << "Impossible to calculate remaining bytes of the response frame" << std::endl;
        return 0;
    }
    else
    {
        responseBuffer[4] = totalRemainingLengthHighByte; // Length (High byte)
        responseBuffer[5] = totalRemainingLengthLowByte;  // Length (Low byte)
    }

    // Free the resources
    delete[] payload;

    return responseIndex; // Return the total length of the response
}


// Function to handle incoming Modbus messages
void ModbusServer::receiveMessages()
{  
    std::lock_guard<std::mutex> lock(slavemutex);
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];  // Buffer to store incoming Modbus queries
    int master_socket;                         // Socket descriptor for the master socket
    int rc;                                    // Return code for various operations
    fd_set rdset;                              // Working set of socket descriptors for select()
    // Initialize the socket set for select()
    FD_ZERO(&refset);  // Clear all entries from the set to initialize it.
                       //This ensures that the set is empty before we add any sockets to it.
    FD_SET(m_modbusSocket, &refset);  // Add the server socket (m_modbusSocket) to the set. 
                                      //This allows the select() function to monitor this socket for incoming connections or data.
    fdmax = m_modbusSocket;           // Set the maximum file descriptor number to the server socket's descriptor.
                                      //This is used in the select() function to specify the range of file descriptors to be monitored.                                     
    socklen_t addrlen;                // Declare a variable 'addrlen' of type 'socklen_t' to hold the size of the client address structure.  
    struct sockaddr_in clientaddr;    // Declare a structure 'clientaddr' of type 'sockaddr_in' to store the client's address information.
    // Infinite loop to keep the server running
    while(true)
    {

        rdset = refset;
        // Use select() to wait for activity on any of the sockets
        if (select(fdmax + 1, &rdset, NULL, NULL, NULL) == -1)
        {
            perror("Server select() failure.");
            break;
        }
        // Loop through all the sockets to find the one that has activity
        for (master_socket = 0; master_socket <= fdmax; master_socket++)
        {
            if (!FD_ISSET(master_socket, &rdset))
            {
                continue;
            }

            // Handle new client connections
            if (master_socket == m_modbusSocket)
            {
                // Declare an integer 'newfd' to hold the new socket file descriptor for the accepted client connection. 
                int newfd;  
                // Initialize 'addrlen' with the size of 'clientaddr'. This is required for the 'accept()' function. 
                addrlen = sizeof(clientaddr);  
                // Initialize all bytes in 'clientaddr' to zero. This is to ensure that it doesn't contain any garbage values before we use it.
                memset(&clientaddr, 0, sizeof(clientaddr)); 
                // Call the 'accept()' function to accept a new client connection.
                // 'm_modbusSocket' is the listening socket, 'clientaddr' will be filled with the client's address information,
                // and 'addrlen' is the size of 'clientaddr'. 
                newfd = accept(m_modbusSocket, (struct sockaddr *)&clientaddr, &addrlen);  
                if (newfd == -1)
                {
                    perror("Server accept() error");
                }
                else
                {
                    FD_SET(newfd, &refset);
                    if (newfd > fdmax)
                    {
                        fdmax = newfd;
                    }
                    //we received a new connection
                    printf("New connection from %s:%d on socket %d\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                }
            }
            else
            {
                // Handle data from a client
                modbus_set_socket  (ctx, master_socket);
                rc = modbus_receive(ctx, query);
                
                if (rc > 0)
                {                    
                    // Parse the received query
                    uint8_t  functionCode;
                    uint16_t startingAddress;
                    uint16_t quantity;
                    if (parseReceivedData(query, rc, functionCode, startingAddress, quantity))
                    {
                        // Handle the parsed data based on the function code
                    }
                    else
                    {
                        std::cerr << "Failed to parse Modbus query." << std::endl;
                    }
                    showTCPFrameForDebugPurpose(query,functionCode,startingAddress,quantity,rc);
                    //From here we have the absolute minimum datas to understand the client request
                    //we need then to generate an answer
                    // Perform action based on function code
                    uint8_t responseBuffer[4096];
                    int    responseLength = 0;
                    switch (functionCode) 
                    {

                        case 0x03: // Read Holding Registers
                        {
                            // Call the function to read holding registers and store the response length
                            int responseLength = readHoldingRegisters(
                                                                        query[0], //high value of transcation ID   1 byte |
                                                                        query[1], //low value of transaction ID    1 byte |___> total 2 bytes (unint16_t type can do the job) 
                                                                        
                                                                        query[2], //high value of the protocol ID  1 byte |
                                                                        query[3], //low value of the protcol ID    1 byte |__> total 2 bytes 
                                                                        query[7], //function code 1 byte
        
                                                                        startingAddress, //2 bytes (uint16_t)
                                                                        
                                                                        quantity,        //2 bytes (uint16_t) 
                                                                        responseBuffer,  //this buffer will be filled with values 
                                                                        ctx);    
                            // Check if the read operation was successful
                            if (responseLength > 0)
                            {
                                // Use the custom SendData function to send the response back to the client
                                ssize_t bytes_sent = sendData(master_socket, responseBuffer, responseLength, clientaddr);
                                // Check if the send operation was successful
                                if (bytes_sent == -1)
                                {
                                    std::cerr << "Failed to send Modbus response using SendData." << std::endl;
                                }
                            }
                            else
                            {
                                std::cerr << "Failed to read holding registers." << std::endl;
                            }
                            break;
                        }

                        case 0x04: //read Input registers
                        {
                            std::cout<<std::endl;
                            std::cout<<"*************************************"<<std::endl;
                            std::cout<<"* Preparing response for the client *"<<std::endl;
                            std::cout<<"*************************************"<<std::endl;
                            std::cout<<std::endl;
                            //the next fuction is the core of the response generation, it will recreate an header and a payload
                            //inside a buffer ready to be sended back on the socket
                            responseLength = readInputRegisters(
                                                                query[0], //high value of transcation ID   1 byte |
                                                                query[1], //low value of transaction ID    1 byte |___> total 2 bytes (unint16_t type can do the job) 
                                                                
                                                                query[2], //high value of the protocol ID  1 byte |
                                                                query[3], //low value of the protcol ID    1 byte |__> total 2 bytes 
                                                                query[7], //function code 1 byte

                                                                startingAddress, //2 bytes (uint16_t)
                                                                
                                                                quantity,        //2 bytes (uint16_t) 
                                                                responseBuffer,  //this buffer will be filled with values 
                                                                ctx
                                                               );
                            std::cout<<"   response length: "<<std::to_string(responseLength)<<std::endl;
                            if (responseLength > 0)
                            {
                                std::cout<<"ready to send data response buffer:"<<std::endl;
                                for (int i=0;i<responseLength;++i)
                                {
                                    std::cout<<"0x"<< static_cast<int>(responseBuffer[i]) << " ";

                                }
                                std::cout<<std::endl;  
                                ssize_t bytes_sent = sendData(master_socket, responseBuffer, responseLength, clientaddr);
                                if (bytes_sent == -1)
                                {
                                    std::cerr << "Failed to send Modbus response using SendData." << std::endl;
                                }
                            }
                            else
                            {
                                std::cerr << "Failed to read input registers." << std::endl;
                            }
                            break;
                           //TODO
                           break;
                        }


                        case 0x06: // Write Single Register
                        {
                           // responseLength = writeSingleRegister(startingAddress, quantity, responseBuffer);
                            break;
                        }
                        // Add more cases for other function codes
                        default:
                        {
                            // Handle unknown function code
                            break;
                        }
                    }

                }
                else if (rc == -1)
                {
                    printf("Connection closed on socket %d\n", master_socket);
                    close(master_socket);
                    FD_CLR(master_socket, &refset);
                    if (master_socket == fdmax)
                    {
                        fdmax--;
                    }
                }
            }
        }
    }
    // Set the initialized flag to false if the loop breaks
    m_initialized = false;
}

ssize_t ModbusServer::sendData(int socket_fd, const uint8_t *data, size_t length, const sockaddr_in &clientaddr)
{
  // Lock the mutex to ensure thread safety
 // std::lock_guard<std::mutex> lock(slavemutex);

  // Use the sendto() system call to send data
  ssize_t bytes_sent = sendto(socket_fd, data, length, 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));

  // Check for errors
  if (bytes_sent == -1) {
      perror("SendData failed");
      return -1;
  }
  return bytes_sent; 
}


bool ModbusServer::loadConfig()
{
    return false;
}
