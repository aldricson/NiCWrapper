#ifndef QSIGNALTEST_H
#define QSIGNALTEST_H

#include <iostream>
#include "..\Ni modules definitions\NIDeviceModule.h"

class QSignalTest {
public:
    void onIntValueChanged(unsigned int newValue,NIDeviceModule *sender) {
        std::cout << "Signal received! Value changed to " << newValue << std::endl;
        std::cout << "Sender alias is  " << sender->getAlias().c_str() << std::endl;
    }
};

#endif // QSIGNALTEST_H
