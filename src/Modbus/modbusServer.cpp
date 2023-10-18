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
    std::cout<<"create modbus context: listening on "<<Host_Ip<<" port:"<<std::to_string(port)<<std::endl;
    // Create a new Modbus context for TCP/IPv4
    ctx = modbus_new_tcp(Host_Ip.c_str(), port);
    std::cout<<"set modbus debug flag to "<<std::to_string(debugging)<<std::endl;
    // Enable or disable debugging based on the parameter
    modbus_set_debug(ctx, debugging);
    // Check if the Modbus context was successfully created
    if (ctx == NULL)
    {
        fprintf(stderr, "There was an error allocating the Modbus context\n");
        throw -1;
    }
    std::cout<<"modbus context set with SUCCES"<<std::endl;
    // Listen for incoming Modbus requests on the created context
    std::cout<<"initialize modbus tcp socket in listen mode"<<std::endl;
    m_modbusSocket = modbus_tcp_listen(ctx, 1);
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
    m_initialized = true;
    return true;
}


ModbusServer::ModbusServer(string host, uint16_t port)
{
    initModbus(host, port, false);
    //TODO：
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
    // Create a new thread to handle the Modbus server's operations
    std::thread loop([this]()
    {
        // Run an infinite loop to keep the server running
        while (true)
        {
            // Check if the Modbus server is initialized
            if (m_initialized)
            {
                // If initialized, call the function to receive messages from clients
                recieveMessages();
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
void ModbusServer::recieveMessages()
{
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int master_socket;
    int rc;
    fd_set refset;
    fd_set rdset;
    int fdmax;
    // Initialize the socket set for select()
    FD_ZERO(&refset);
    FD_SET(m_modbusSocket, &refset);
    fdmax = m_modbusSocket;
    while(true)
    {
        rdset = refset;
        // Use select() to wait for an activity on any of the sockets
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

            if (master_socket == m_modbusSocket)
            {
                // Handle new client connections
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;
                addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));
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
                    printf("New connection from %s:%d on socket %d\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                }
            }
            else
            {
                // Handle data from a client
                modbus_set_socket(ctx, master_socket);
                rc = modbus_receive(ctx, query);
                
                if (rc > 0)
                {
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
    m_initialized = false;
}
