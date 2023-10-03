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

struct MappingData {
    int index;
    ModuleIo moduleIo;
    ModuleType moduleType;
    std::string moduleSource;
    std::string channelSource;
    float minSource;
    float maxSource;
    int minDestination;
    int maxDestination;
    int destinationModbusChannel;
};

#endif