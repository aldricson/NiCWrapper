#ifndef GLOBALENUMSTRUCTS_H
#define GLOBALENUMSTRUCTS_H

enum class ModuleIo {
    INPUT = 0,
    OUTPUT = 1
};

enum class ModuleType {
    ANALOG = 0,
    DIGITAL = 1,
    COUNTER = 2,
    CODER = 3
};

struct MappingConfig {
    int index;
    ModuleType moduleType;
    std::string module;
    std::string channel;
    float minSource;
    float maxSource;
    uint16_t minDest;
    uint16_t maxDest;
    int modbusChannel;

    MappingConfig() : index(0), moduleType(ModuleType::ANALOG), minSource(0.0f), maxSource(0.0f), 
                      minDest(0), maxDest(0), modbusChannel(0) {}
};

#endif