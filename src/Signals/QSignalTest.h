#ifndef QSIGNALTEST_H
#define QSIGNALTEST_H

#include <iostream>

class QSignalTest {
public:
    void onIntValueChanged(int newValue) {
        std::cout << "Signal received! Value changed to " << newValue << std::endl;
    }
};

#endif // QSIGNALTEST_H
