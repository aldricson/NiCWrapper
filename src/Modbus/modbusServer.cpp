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
    mapping = modbus_mapping_new(m_numBits, m_numInputBits, m_numInputRegisters, m_numRegisters);
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
}

ModbusServer::~ModbusServer()
{
    modbus_mapping_free(mapping);
    modbus_close(ctx);
    modbus_free(ctx);
}

void ModbusServer::loadFromConfigFile()
{
    return;
}

// Run the Modbus server in a separate thread to continuously receive messages
void ModbusServer::run()
{
    std::cout<<"ready to launch thread"<<std::endl;
    // Create a new thread to handle the Modbus server's operations
    std::thread loop([this]()
    {
        std::cout<<"thread loop"<<std::endl;
        // Run an infinite loop to keep the server running
        while (!m_stopRequested.load())  // Check the stop flag using load()
        {
            // Check if the Modbus server is initialized
            if (m_initialized)
            {
                 std::cout<<"check for init ok"<<std::endl;
                // If initialized, call the function to receive messages from clients
                receiveMessages();
            }
            else
            {
                // If not initialized, set the initialization flag to true
                // Note: This is a placeholder, proper initialization should be done
                m_initialized = true;
            }
        }
    });
    
    // Detach the thread so it runs independently of the main thread
    loop.detach();
    return;
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


// Set the Modbus slave ID for the server
bool ModbusServer::modbus_set_slave_id(int id)
{
    // Call the libmodbus function to set the slave ID
    int rc = modbus_set_slave(ctx, id);
    
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

// Set a 16-bit value in the Modbus server's input register
bool ModbusServer::setInputRegisterValue(int registerStartaddress, uint16_t Value)
{
    // Check if the register address is within the valid range
    if (registerStartaddress > (m_numRegisters - 1))
    {
        return false;
    }
    // Lock the mutex to ensure thread safety
    slavemutex.lock();
    // Set the value in the input register
    mapping->tab_input_registers[registerStartaddress] = Value;
    // Unlock the mutex
    slavemutex.unlock();
    return true;
}

// Initialize the Modbus server's holding register with a 16-bit unsigned integer value
bool ModbusServer::setHoldingRegisterValue(int registerStartaddress, uint16_t Value)
{
    // Check if the register address is within the valid range
    if (registerStartaddress > (m_numRegisters - 1))
    {
        return false;
    }
    // Lock the mutex to ensure thread safety
    slavemutex.lock();
    // Set the value in the holding register
    mapping->tab_registers[registerStartaddress] = Value;
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

void ModbusServer::updateSimulatedModbusAnalogRegisters(NItoModbusBridge *bridge)
{
    std::vector<u_int16_t> latestData = bridge->getLatestSimulatedData();
    if (latestData.size() != 64) 
    { // Safety check
        return;
    }
    for (int i = 0; i < 64; ++i) 
    {
        setHoldingRegisterValue(i, latestData[i]);
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

// Function to handle incoming Modbus messages
void ModbusServer::receiveMessages()
{
    
    std::lock_guard<std::mutex> lock(slavemutex);
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];  // Buffer to store incoming Modbus queries
    int master_socket;  // Socket descriptor for the master socket
    int rc;  // Return code for various operations
    fd_set rdset;  // Working set of socket descriptors for select()
    // Initialize the socket set for select()
    FD_ZERO(&refset);  // Clear all entries from the set to initialize it.
                       //This ensures that the set is empty before we add any sockets to it.
    FD_SET(m_modbusSocket, &refset);  // Add the server socket (m_modbusSocket) to the set. 
                                       //This allows the select() function to monitor this socket for incoming connections or data.
    fdmax = m_modbusSocket;  // Set the maximum file descriptor number to the server socket's descriptor.
                             //This is used in the select() function to specify the range of file descriptors to be monitored.
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

                 // Declare a variable 'addrlen' of type 'socklen_t' to hold the size of the client address structure. 
                socklen_t addrlen;
                // Declare a structure 'clientaddr' of type 'sockaddr_in' to store the client's address information. 
                struct sockaddr_in clientaddr;
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
                modbus_set_socket(ctx, master_socket);
                rc = modbus_receive(ctx, query);
                
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
                if (rc > 0)
                {
                    std::cout << "Received Modbus query frame: ";
                    for (int i = 0; i < rc; ++i)
                    {
                        std::cout << std::hex << static_cast<int>(query[i]) << " ";
                    }
                    std::cout << std::endl;
                    //Human-readable breakdown of the Modbus query frame
                    std::cout << "Human-readable breakdown:" << std::endl;
                    std::cout << "  Transaction Identifier: " << std::hex << static_cast<int>(query[0]) << " " << static_cast<int>(query[1]) << std::endl;
                    std::cout << "  Protocol Identifier: " << std::hex << static_cast<int>(query[2]) << " " << static_cast<int>(query[3]) << std::endl;
                    std::cout << "  Length Field: " << std::hex << static_cast<int>(query[4]) << " " << static_cast<int>(query[5]) << std::endl;
                    std::cout << "  Unit Identifier: " << std::hex << static_cast<int>(query[6]) << std::endl;
                    std::cout << "  Function Code: " << std::hex << static_cast<int>(query[7]) << std::endl;
                    std::cout << "  Data: ";
                    for (int i = 8; i < rc; ++i)
                    {
                       std::cout << std::hex << static_cast<int>(query[i]) << " ";
                    }
                    std::cout << std::endl;
                    
                    // Reply to the Modbus query
                    modbus_reply(ctx, query, rc, mapping);
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
