#ifndef STARTUPMANAGER_H
#define STARTUPMANAGER_H

#include <string>
#include <functional>
#include <memory>
#include "./filesUtils/iniObject.h"

class StartUpManager {
public:
    StartUpManager();
    
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);

    // Getters
    int getNiPollingOrder() const;
    int getModbusOrder() const;
    bool isTestSequenceOn() const;
    bool isNiPollingOn() const;
    bool isModBusOn() const;

    // Setters
    void setNiPollingOrder(int order);
    void setModbusOrder(int order);
    void setTestSequenceOn(bool state);
    void setNiPollingOn(bool state);
    void setModBusOn(bool state);

    // Signals
    std::function<void(int, StartUpManager*)> niPollingOrderChangedSignal;
    std::function<void(int, StartUpManager*)> modbusOrderChangedSignal;
    std::function<void(bool, StartUpManager*)> testSequenceOnChangedSignal;
    std::function<void(bool, StartUpManager*)> niPollingOnChangedSignal;
    std::function<void(bool, StartUpManager*)> modBusOnChangedSignal;

    std::shared_ptr<IniObject> m_ini;
private:
    int m_niPollingOrder;
    int m_modbusOrder;
    bool m_testSequenceOn;
    bool m_niPollingOn;
    bool m_modBusOn;
};

#endif // STARTUPMANAGER_H
