#include "startUpManager.h"
#include <fstream>
#include <iostream>
#include <cstdio>

StartUpManager::StartUpManager() {
    std::ifstream ini("startup.ini");
    if (ini.is_open()) {
        loadFromFile("startup.ini");
    } else {
        // Initialize with default values
        m_niPollingOrder = 0;
        m_modbusOrder = 1;
        m_testSequenceOn = true;
        m_niPollingOn = true;
        m_modBusOn = true;
        saveToFile("startup.ini");
    }
}

void StartUpManager::loadFromFile(const std::string& filename) {
    IniParser parser(filename);

    // Init Order section
    m_niPollingOrder = parser.readInteger("Init Order", "NiPolling");
    m_modbusOrder = parser.readInteger("Init Order", "Modbus");

    // startUp section
    m_testSequenceOn = (parser.readString("startUp", "testSequenceOn") == "true");
    m_niPollingOn = (parser.readString("startUp", "NiPollingOn") == "true");
    m_modBusOn = (parser.readString("startUp", "ModBusOn") == "true");
}

void StartUpManager::saveToFile(const std::string& filename) {
    IniParser parser(filename);

    // Init Order section
    parser.writeInteger("Init Order", "NiPolling", m_niPollingOrder);
    parser.writeInteger("Init Order", "Modbus", m_modbusOrder);

    // startUp section
    parser.writeString("startUp", "testSequenceOn", m_testSequenceOn ? "true" : "false");
    parser.writeString("startUp", "NiPollingOn", m_niPollingOn ? "true" : "false");
    parser.writeString("startUp", "ModBusOn", m_modBusOn ? "true" : "false");

    // Save to file
    parser.save();
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
