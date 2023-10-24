#ifndef MODBUS_SERVER_H
#define MODBUS_SERVER_H

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <iostream>
#include <mutex>
#include <string>
#include <atomic>
using namespace std;
#include <modbus.h>
#include <unistd.h>
#include <error.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include "modbusServerUtils.h"
#include "../stringUtils/stringUtils.h"


//forward definition
class NItoModbusBridge;

class ModbusServer
{
public:
    ModbusServer(string host="0.0.0.0", uint16_t port=502);
    ~ModbusServer();

public:
    void showTCPFrameForDebugPurpose(const uint8_t *query, const uint8_t functionCode, const uint16_t startingAddres, const uint16_t quantity,const int rc);
    bool parseReceivedData(const uint8_t* buffer, int bytesRead, uint8_t& functionCode, uint16_t& startingAddress, uint16_t& quantity);
    
    // Function to handle reading of holding registers (Function Code 0x03)
    int  readHoldingRegisters(uint8_t transactionIdHigh, uint8_t transactionIdLow,
                              uint8_t protocolIdHigh   , uint8_t protocolIdLow,
                              uint8_t functionCode     ,
                              uint16_t startingAddress ,
                              uint16_t quantity        ,
                              uint8_t *responseBuffer  ,
                              modbus_t *ctx);
                          
    bool calculateRemainingBytes(const int totalLengthInBytes,
                                 uint8_t &highByte,
                                 uint8_t &lowByte); 

    int readCoils(
        uint8_t transactionIdHigh, uint8_t transactionIdLow,
        uint8_t protocolIdHigh, uint8_t protocolIdLow,
        uint8_t functionCode, uint16_t startingAddress,
        uint16_t quantity, uint8_t *responseBuffer, modbus_t *ctx);

    int writeSingleCoil(
    uint8_t transactionIdHigh, uint8_t transactionIdLow,
    uint8_t protocolIdHigh, uint8_t protocolIdLow,
    uint8_t functionCode, uint16_t coilAddress,
    bool value, uint8_t *responseBuffer, modbus_t *ctx);

    int  readInputRegisters(
                            uint8_t transactionIdHigh , uint8_t transcationIdLow,
                            uint8_t protocolIdHigh    , uint8_t protocolIdLow,
                            uint8_t functionCode      ,
                            uint16_t startingAddress  ,
                            uint16_t quantity         ,
                            uint8_t *responseBuffer   ,
                            modbus_t *ctx);
                             
    void receiveMessages();
    // Validate the starting address and quantity for reading registers
    bool validateAddressAndQuantity(uint16_t startingAddress, uint16_t quantity, uint16_t maxRegisters);
    //function to generate a near valid modbus header still length are calculated after the payload buffer is filled
    void prepareResponseHeader(
                                uint8_t transactionIdHigh,
                                uint8_t transactionIdLow,
                                uint8_t protocolIdHigh,
                                uint8_t protocolIdLow,
                                uint8_t functionCode,
                                uint16_t quantity,
                                uint8_t *responseBuffer);
    // function to fill the response payload for Modbus TCP frame
    int fillResponsePayload(uint16_t *payload, uint16_t quantity, uint8_t *responseBuffer, int startIndex);
    // Overloaded function to fill response payload for uint8_t (for example for coils)
    int fillResponsePayload(uint8_t *payload, int payloadSize, uint8_t *responseBuffer, int startIndex);
    // Function to send data to a specific client
    ssize_t sendData(int socket_fd, const uint8_t *data, size_t length, const struct sockaddr_in &clientaddr);
    
    bool modbus_set_slave_id(int id);
    bool initModbus(std::string Host_Ip, int port, bool debugging);
    
    uint8_t getTab_Input_Bits(int NumBit);
    bool setTab_Input_Bits(int NumBit, uint8_t Value);

    uint16_t getHoldingRegisterValue(int registerNumber);
    float getHoldingRegisterFloatValue(int registerStartaddress);
    
    bool setHoldingRegisterValue(int registerNumber, uint16_t Value);
    bool setHoldingRegisterValue(int registerNumber, float Value);

    bool setInputRegisterValue(int registerNumber, uint16_t Value);
    bool setInputRegisterValue(int registerNumber, float Value);

    // This function will be set as the slot function for newSimulationBufferReadySignal //nb for compatibility reasons, we have 64 channels
    //in analog registers
    void updateSimulatedModbusAnalogRegisters(NItoModbusBridge *bridge);
    

private:
    fd_set refset;
    int fdmax;
    std::mutex slavemutex;
    uint8_t    m_slave_Id;
    int m_errCount{ 0 };
    int m_modbusSocket{ -1 };
    bool m_initialized{ false };
    modbus_t* ctx{ nullptr };

    std::atomic<bool> m_stopRequested;  // Flag to request server stop

/*********************************************************
 * modbus_mapping_t explainations
 * 
typedef struct _modbus_mapping_t
{
    int nb_bits;                // Coils
    int start_bits;
    int nb_input_bits;          // Discrete Inputs
    int start_input_bits;
    int nb_input_registers;     // Input Registers
    int start_input_registers;
    int nb_registers;           // Holding Registers
    int start_registers;
    uint8_t *tab_bits;
    uint8_t *tab_input_bits;
    uint16_t *tab_input_registers;
    uint16_t *tab_registers;
} modbus_mapping_t;  */

    modbus_mapping_t* mapping{ nullptr };
    /*Mapping*/
    int m_nbCoils          { 60000 };
    int m_numInputBits     { 60000 };
    int m_numRegisters     { 60000 };
    int m_numInputRegisters{ 60000 };

public:
    void loadFromConfigFile();
    void run();
    void stop();
    bool isModbusActive();

};

#endif 