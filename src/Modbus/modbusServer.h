#ifndef MODBUS_SERVER_H
#define MODBUS_SERVER_H

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <iostream>
#include <mutex>
#include <string>
using namespace std;
#include <modbus.h>
#include <unistd.h>
#include <error.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include "modbusServerUtils.h"


//forward definition
class NItoModbusBridge;

class ModbusServer
{
public:
    ModbusServer(string host="0.0.0.0", uint16_t port=502);
    ~ModbusServer();

public:
    void recieveMessages();
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
    std::mutex slavemutex;
    int m_errCount{ 0 };
    int m_modbusSocket{ -1 };
    bool m_initialized{ false };
    modbus_t* ctx{ nullptr };

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
    int m_numBits          { 60000 };
    int m_numInputBits     { 60000 };
    int m_numRegisters     { 60000 };
    int m_numInputRegisters{ 60000 };

public:
    void loadFromConfigFile();
    void run();
};

#endif 
