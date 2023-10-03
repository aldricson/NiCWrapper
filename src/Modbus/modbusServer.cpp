/*#include "ModbusServer.h"
#include <iostream>
#include <errno.h>
#include <mutex>

std::mutex mtx; // Mutex for thread safety

ModbusServer::ModbusServer() : running_(false), ctx_(nullptr), mb_mapping_(nullptr) {
    // Initialize Modbus context (TCP, port 1502)
    ctx_ = modbus_new_tcp("127.0.0.1", 1502);

    if (ctx_ == nullptr) {
        std::cerr << "Unable to create the libmodbus context" << std::endl;
        return;
    }

    // Create Modbus mapping with 64 holding registers
    mb_mapping_ = modbus_mapping_new(0, 0, 64, 0);
    if (mb_mapping_ == nullptr) {
        std::cerr << "Failed to create Modbus mapping" << std::endl;
        modbus_free(ctx_);
        ctx_ = nullptr;
        return;
    }
}

ModbusServer::~ModbusServer() {
    std::lock_guard<std::mutex> lock(mtx);
    modbus_mapping_free(mb_mapping_);
    modbus_free(ctx_);
}

void ModbusServer::Start() {
    std::lock_guard<std::mutex> lock(mtx);
    running_ = true;
    serverThread_ = std::thread(&ModbusServer::ServerThread, this);
}

void ModbusServer::Stop() {
    std::lock_guard<std::mutex> lock(mtx);
    running_ = false;
    serverThread_.join();
}

void ModbusServer::ServerThread() {
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    modbus_set_debug(ctx_, TRUE);

    int server_socket = modbus_tcp_listen(ctx_, 1);  // Corrected this line
    if (server_socket == -1) {
        std::cerr << "Failed to listen: " << modbus_strerror(errno) << std::endl;
        return;
    }

    while (true) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!running_) break;
        }

        modbus_set_slave(ctx_, -1); // Allow connections from any slave ID
        int rc = modbus_receive(ctx_, query);

        if (rc >= 0) {
            modbus_reply(ctx_, query, rc, mb_mapping_);
        } else {
            std::cerr << "Failed to receive: " << modbus_strerror(errno) << std::endl;
        }
    }
}*/
