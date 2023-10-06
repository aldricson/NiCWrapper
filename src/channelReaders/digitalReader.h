#ifndef DigitalReader_H
#define DigitalReader_H

#include "baseReader.h"

class DigitalReader : public BaseReader {
public:
    // Constructor
    DigitalReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance,
                  std::shared_ptr<QNiDaqWrapper> aDaqMxInstance);
    
    // Override the pure virtual functions
    void manualReadOneShot() override;
    void manualReadOneShot(double &returnedValue) override;
    void manualReadPolling() override;
    void displayChooseModuleMenu() override;
    
    // Add any additional member functions specific to DigitalReader here
};

#endif // DigitalReader_H
