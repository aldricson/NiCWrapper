#include "startUpManager.h"
#include <fstream>
#include <iostream>
#include <cstdio>

StartUpManager::StartUpManager() {
    m_ini = std::make_shared<IniObject>();

    std::ifstream ini("startup.ini");
    if (ini.is_open()) {
        loadFromFile("startup.ini");
    } 
    else 
    {
        // Initialize with default values
        m_niPollingOrder = 0;
        m_modbusOrder = 1;
        m_testSequenceOn = true;
        m_niPollingOn = true;
        m_modBusOn = true;
        saveToFile("startup.ini");
    }
}

void StartUpManager::loadFromFile(const std::string& filename) 
{
   //check the INI file
   FILE* fp = std::fopen(filename.c_str(), "r");
   if (fp == nullptr) {
       // File doesn't exist; consider creating a default one
       // ... (populate ini with default values)
       std::fclose(fp);
       saveToFile(filename);
   }
   std::fclose(fp);

   //---------- Section init order ------------
   m_niPollingOrder =  m_ini->readInteger("InitOrder","NiPolling",0, filename);
   m_modbusOrder    =  m_ini->readInteger("InitOrder","Modbus",1, filename);
   //------------  section startup -----------------
   m_testSequenceOn = m_ini->readBoolean("startUp","testSequenceOn",true, filename);
   m_niPollingOn    = m_ini->readBoolean("startUp","NiPollingOn"   ,true, filename);
   m_modBusOn       = m_ini->readBoolean("startUp","ModBusOn"      ,true, filename);
}

void StartUpManager::saveToFile(const std::string& filename) 
{
    // Open and read the existing INI file, if it exists
    FILE* fp = std::fopen(filename.c_str(), "r");
    if (fp != nullptr) {
        std::fclose(fp);
        loadFromFile(filename);
    }
    std::fclose(fp);
    //---------- Section init order ------------
    m_ini->writeInteger("InitOrder","NiPolling",m_niPollingOrder, filename);
    m_ini->writeInteger("InitOrder","Modbus",   m_modbusOrder, filename   );
    //------------  section startup -----------------
    m_ini->writeBoolean("startUp","testSequenceOn",m_testSequenceOn, filename);
    m_ini->writeBoolean("startUp","NiPollingOn"   ,m_niPollingOn, filename);
    m_ini->writeBoolean("startUp","ModBusOn"      ,m_modBusOn, filename);


}

// Getters
int StartUpManager::getNiPollingOrder() const { return m_niPollingOrder; }
int StartUpManager::getModbusOrder() const { return m_modbusOrder; }
bool StartUpManager::isTestSequenceOn() const { return m_testSequenceOn; }
bool StartUpManager::isNiPollingOn() const { return m_niPollingOn; }
bool StartUpManager::isModBusOn() const { return m_modBusOn; }

// Setters
void StartUpManager::setNiPollingOrder(int order) {
    m_niPollingOrder = order;
    if (niPollingOrderChangedSignal) {
        niPollingOrderChangedSignal(order, this);
    }
}

void StartUpManager::setModbusOrder(int order) {
    m_modbusOrder = order;
    if (modbusOrderChangedSignal) {
        modbusOrderChangedSignal(order, this);
    }
}

void StartUpManager::setTestSequenceOn(bool state) {
    m_testSequenceOn = state;
    if (testSequenceOnChangedSignal) {
        testSequenceOnChangedSignal(state, this);
    }
}

void StartUpManager::setNiPollingOn(bool state) {
    m_niPollingOn = state;
    if (niPollingOnChangedSignal) {
        niPollingOnChangedSignal(state, this);
    }
}

void StartUpManager::setModBusOn(bool state) {
    m_modBusOn = state;
    if (modBusOnChangedSignal) {
        modBusOnChangedSignal(state, this);
    }
}
