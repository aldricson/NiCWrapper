#include "NewModbusServer.h"
#include <iostream>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


NewModbusServer::NewModbusServer()
    : ctx(nullptr), mb_mapping(nullptr), server_socket(-1), fdmax(0)
{
    // Initialize the reference set for socket descriptors
    FD_ZERO(&refset);

    // Create a shared pointer for IniObject
    m_ini = std::make_shared<IniObject>();

    // Load configuration from an INI file
    loadConfig();

    // Initialize the modbus context and handle any errors
    initializeModbusContext();

    // Setup the server socket and handle any errors
    setupServerSocket();
  
}

NewModbusServer::~NewModbusServer() {
    // Check if the server_socket is valid and close it
    if (server_socket != -1) {
        close(server_socket);
    }

    // Free the modbus context
    if (ctx != nullptr) {
        modbus_free(ctx);
    }

    // Free the modbus mapping
    if (mb_mapping != nullptr) {
        modbus_mapping_free(mb_mapping);
    }
}

void NewModbusServer::loadConfig()
{
    bool ok;
    try 
    {
        // Read and update the 'm_modeSRU' setting from the configuration file
        SRUMapping.m_modeSRU = m_ini->readBoolean("exlog", "compatibilitylayer", SRUMapping.m_modeSRU, fileName,ok);
        // Read and update the 'm_nbSRUAnalogsIn' setting from the configuration file
        SRUMapping.m_nbSRUAnalogsIn = m_ini->readInteger("exlogmapping", "nbanalogsin", SRUMapping.m_nbSRUAnalogsIn, fileName,ok);
        // Read and update the 'm_nbSRUAnalogsOut' setting from the configuration file
        SRUMapping.m_nbSRUAnalogsOut = m_ini->readInteger("exlogmapping", "nbanalogsout", SRUMapping.m_nbSRUAnalogsOut, fileName,ok);
        // Read and update the 'm_nbSRUCounters' setting from the configuration file
        SRUMapping.m_nbSRUCounters = m_ini->readInteger("exlogmapping", "nbcounters", SRUMapping.m_nbSRUCounters, fileName,ok);
        // Read and update the 'm_nbSRUAlarms' setting from the configuration file
        SRUMapping.m_nbSRUAlarms = m_ini->readInteger("exlogmapping", "nbalarms", SRUMapping.m_nbSRUAlarms, fileName,ok);
    } 
    catch (const std::exception& e) 
    {
        // Handle any exceptions that might occur during configuration loading
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
    }
}

void NewModbusServer::initializeModbusContext() {
    // Initialize modbus context with TCP protocol listening on "0.0.0.0" and port 502
    ctx = modbus_new_tcp("0.0.0.0", 502);

    // Check for context initialization errors
    if (ctx == nullptr) {
        std::cerr << "Failed to initialize modbus context: " << modbus_strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create an internal modbus mapping
    mb_mapping = modbus_mapping_new(20, 20, 512, 512);

    // Check for mapping allocation errors
    if (mb_mapping == nullptr) {
        std::cerr << "Failed to allocate the mapping: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx); // Free the context before exiting
        exit(EXIT_FAILURE);
    }

    // Additional check for tab_input_registers
    if (!mb_mapping->tab_input_registers) {
        std::cerr << "Failed to allocate tab_input_registers." << std::endl;
        modbus_mapping_free(mb_mapping); // Free the mapping before exiting
        modbus_free(ctx); // Free the context before exiting
        exit(EXIT_FAILURE);
    }
}

void NewModbusServer::setupServerSocket() {
    // Create a TCP socket and listen for incoming connections
    server_socket = modbus_tcp_listen(ctx, NB_CONNECTION);

    // Check for socket setup errors
    if (server_socket == -1) {
        std::cerr << "Unable to listen TCP connection: " << modbus_strerror(errno) << std::endl;

        // Free the modbus context before exiting
        modbus_free(ctx);
        exit(EXIT_FAILURE);
    }

    // Register a signal handler for SIGINT (Ctrl+C) to gracefully exit the server
    signal(SIGINT, NewModbusServer::closeServer);

    // Add the server socket to the reference set for select()
    FD_SET(server_socket, &refset);

    // Update fdmax with the server socket descriptor
    fdmax = server_socket;
}

void NewModbusServer::runServer() {
    while (true) {
        // Create a copy of the reference set for select()
        fd_set rdset = refset;

        // Use select() to wait for activity on sockets
        if (select(fdmax + 1, &rdset, NULL, NULL, NULL) == -1) {
            perror("Server select() failure.");
            closeServer(EXIT_FAILURE); // Handle select() failure and exit
        }
         // Iterate through all possible sockets
        for (int master_socket = 0; master_socket <= fdmax; master_socket++) {
            if (!FD_ISSET(master_socket, &rdset)) {
                continue; // Skip sockets with no activity
            }

            if (master_socket == server_socket) {
                // Handle a new connection
                handleNewConnection(); 
            } else {
                // Handle client requests
                handleClientRequest(master_socket);
            }
        }
    }
}

void NewModbusServer::handleNewConnection() {
    // Declare variables for socket address and new file descriptor
    socklen_t addrlen;
    struct sockaddr_in clientaddr;
    int newfd;

    // Set the size of the client address
    addrlen = sizeof(clientaddr);

    // Initialize the client address structure to zero
    memset(&clientaddr, 0, sizeof(clientaddr));

    // Accept a new connection on the server socket
    newfd = accept(server_socket, (struct sockaddr *) &clientaddr, &addrlen);

    // Check if the new connection is successfully accepted
    if (newfd == -1) {
        // Handle accept() error by printing an error message
        perror("Server accept() error");
    } else {
        // Add the new socket to the reference set for select()
        FD_SET(newfd, &refset);

        // Update the maximum file descriptor number if the new socket is greater
        if (newfd > fdmax) {
            fdmax = newfd;
        }

        // Convert the client's IP address to a string
        std::string ipAddress = inet_ntoa(clientaddr.sin_addr);

        // Add the new client to the client list and broadcast the update
        updateClientList(newfd, ipAddress, false);
        broadcastClientList();
    }
}

void NewModbusServer::updateClientList(int socket, const std::string& ipAddress, bool remove) {
    // Lock the client list mutex to ensure thread safety
    std::lock_guard<std::mutex> lock(clientListMutex);

    // Check if the operation is to remove a client
    if (remove) {
        // Remove the client with the specified socket from the list
        clientList.erase(socket);
    } else {
        // Add or update the client with the specified socket and IP address
        clientList[socket] = ipAddress;
    }
}

void NewModbusServer::broadcastClientList() 
{
    // Create a message to display the connected clients
    std::string message = "cli:Connected Clients: " + std::to_string(clientList.size());

    // Iterate through the client list
    for (const auto& clientPair : clientList) {
        int clientSocket = clientPair.first;        // Extract the socket
        std::string clientIp = clientPair.second;   // Extract the IP address

        // Append client information to the message
        message += "\nSocket " + std::to_string(clientSocket) + ": " + clientIp;
    }

    // At this point, the 'message' variable contains the list of connected clients
    // In the future we may want to perform some action with this message, such as sending it to a specific client or logging it.
}


void NewModbusServer::handleClientRequest(int master_socket) {
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

    // Set the socket for modbus context
    modbus_set_socket(ctx, master_socket);

    // Receive a Modbus request
    int rc = modbus_receive(ctx, query);

    if (rc > 0) {
        // Process the valid Modbus request and send a response
        modbus_reply(ctx, query, rc, mb_mapping);
    } else if (rc == -1) {
        // Connection closed by the client
        std::cout << "Connection closed on socket " << master_socket << std::endl;

        // Update the client list to reflect the disconnection
        updateClientList(master_socket, "", true);  // 'true' indicates removal
        broadcastClientList();

        // Close the socket and remove it from the set
        close(master_socket);
        FD_CLR(master_socket, &refset);

        // Update fdmax if necessary
        if (master_socket == fdmax) {
            // Decrease fdmax to the highest active socket
            fdmax = findMaxSocket();
        }
    }
}


int NewModbusServer::findMaxSocket() {
    int maxSock = server_socket;

    // Iterate through the client list to find the maximum socket
    for (const auto& client : clientList) {
        if (client.first > maxSock) {
            maxSock = client.first;
        }
    }

    // Return the maximum socket found
    return maxSock;
}

void NewModbusServer::closeServer(int signal) {
    // Display a message indicating the reason for server closure
    std::cout << "Closing the server due to signal " << signal << std::endl;
    
    // Exit the server with the provided signal
    exit(signal);
}

void NewModbusServer::reMapInputRegisterValuesForAnalogics(const std::vector<uint16_t>& newValues) {
    // Lock the mutex to ensure thread safety while accessing mb_mapping
    std::lock_guard<std::mutex> lock(mb_mapping_mutex);

    // Check if mb_mapping is valid
    if (!mb_mapping) {
        // If mb_mapping is not valid, return without making any changes
        return;
    }

    // Check if tab_input_registers is valid
    if (!mb_mapping->tab_input_registers) {
        // If tab_input_registers is not valid, return without making any changes
        return;
    }

    // Determine the number of registers to write, ensuring not to exceed the allocated array size
    size_t numRegistersToWrite = std::min(newValues.size(), static_cast<size_t>(MODBUS_MAX_READ_REGISTERS));

    // Copy new values to the input registers
    for (size_t i = 0; i < numRegistersToWrite; ++i) {
        mb_mapping->tab_input_registers[i] = newValues[i];
    }
}

void NewModbusServer::reMapCoilsValues(const std::vector<bool>& newValues) {
    // Lock the mutex to ensure thread safety while accessing mb_mapping
    std::lock_guard<std::mutex> lock(mb_mapping_mutex);

    // Iterate through the new values and update corresponding coils in mb_mapping
    for (size_t i = 0; i < newValues.size() && i < MODBUS_MAX_READ_BITS; ++i) {
        // Check if the index is within bounds of mb_mapping->tab_bits
        if (i < MODBUS_MAX_READ_BITS) {
            // Update the coil value in mb_mapping
            mb_mapping->tab_bits[i] = newValues[i];
        } else {
            // If the index exceeds the bounds, break the loop to prevent accessing invalid memory
            break;
        }
    }
}

bool NewModbusServer::modbusSetSlaveId(int newSlaveId) {
    // Lock the mutex to ensure thread safety when accessing the modbus context (ctx)
    std::lock_guard<std::mutex> lock(ctxMutex);

    // Check if the newSlaveId is within the valid range (0 to 255)
    if (newSlaveId < 0 || newSlaveId > 255) {
        std::cerr << "Invalid slave ID. Must be between 0 and 255." << std::endl;
        return false; // Return false to indicate failure
    }

    // Attempt to set the new slave ID
    if (modbus_set_slave(ctx, newSlaveId) == -1) {
        // If there was an error, retrieve the error message and handle it
        std::string error = modbus_strerror(errno);
        std::cerr << "Failed to set the slave ID: " << error << std::endl;
        return false; // Return false to indicate failure
    }

    return true; // Return true to indicate success
}

// Get the SRU mapping structure with thread-safe access
SensorRigUpStruct NewModbusServer::getSRUMapping() const {
    std::lock_guard<std::mutex> lock(sruMappingMutex);
    return SRUMapping;
}

// Set the SRU mapping structure with thread-safe access
void NewModbusServer::setSRUMapping(const SensorRigUpStruct &newMapping)
{
    std::lock_guard<std::mutex> lock(sruMappingMutex);
    SRUMapping = newMapping;
}
// Calculate and return the size of the SRU mapping without alarms
int NewModbusServer::getSRUMappingSizeWithoutAlarms()
{
    int totalSize =  SRUMapping.m_nbSRUAnalogsIn    +
                     SRUMapping.m_nbSRUAnalogsOut   +
                    (SRUMapping.m_nbSRUCoders   *2) + //2 registers of 16 bits each for each coder
                    (SRUMapping.m_nbSRUCounters *3);  // 3 registers of 16 bits each for each counter (1x frequency, 2x value) 
    return totalSize;
}
