/*#ifndef MODBUS_SERVER_H
#define MODBUS_SERVER_H

#include "libmodbus/modbus.h"
#include <thread>
#include <string>

class ModbusServer {
public:
    ModbusServer();
    ~ModbusServer();

    // Start the Modbus server
    void Start();

    // Stop the Modbus server
    void Stop();

private:
    // The main loop for handling Modbus queries
    void ServerThread();

    // Flag to indicate if the server is running
    bool running_;

    // Modbus context
    modbus_t* ctx_;

    // Modbus mapping
    modbus_mapping_t* mb_mapping_;

    // Thread for running the Modbus server
    std::thread serverThread_;
};

#endif // MODBUS_SERVER_H*/
