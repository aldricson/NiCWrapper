#ifndef MAPPINGGRID_H
#define MAPPINGGRID_H

#include "stringGrid.h"
#include "stringUtils.h"
#include <algorithm> // for std::sort

class NIDeviceModule;

class MappingGrid : public StringGrid {
public:
    void sortChannels();
    void addChannel(NIDeviceModule *aModule, char aChannelName[256]);
};

#endif // MAPPINGGRID_H