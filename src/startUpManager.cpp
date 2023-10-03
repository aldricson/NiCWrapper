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
    IniParser(filename, [this](const std::string& section, const std::string& key, const std::string& value) {
        if (section == "Init Order") {
            if (key == "NiPolling") {
                m_niPollingOrder = std::stoi(value);
            } else if (key == "Modbus") {
                m_modbusOrder = std::stoi(value);
            }
        } else if (section == "startUp") {
            if (key == "testSequenceOn") {
                m_testSequenceOn = (value == "true");
            } else if (key == "NiPollingOn") {
                m_niPollingOn = (value == "true");
            } else if (key == "ModBusOn") {
                m_modBusOn = (value == "true");
            }
        }
    });
}

void StartUpManager::saveToFile(const std::string& filename) {
    FILE* ini;
    if ((ini = fopen(filename.c_str(), "w")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", filename.c_str());
        return;
    }

    fprintf(ini, "#\n# StartUp Configuration\n#\n\n[Init Order]\n\n");
    fprintf(ini, "NiPolling = %d\n", m_niPollingOrder);
    fprintf(ini, "Modbus = %d\n", m_modbusOrder);

    fprintf(ini, "\n[startUp]\n\n");
    fprintf(ini, "testSequenceOn = %s\n", m_testSequenceOn ? "true" : "false");
    fprintf(ini, "NiPollingOn = %s\n", m_niPollingOn ? "true" : "false");
    fprintf(ini, "ModBusOn = %s\n", m_modBusOn ? "true" : "false");

    fclose(ini);
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
