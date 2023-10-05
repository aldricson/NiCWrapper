#include "digitalReader.h"

// Constructor
DigitalReader::DigitalReader(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance,
                             std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
    : BaseReader(aSysConfigInstance, aDaqMxInstance)
{
    // Add any initialization specific to DigitalReader here
}

// Implement the pure virtual functions
void DigitalReader::manualReadOneShot()
{
    // Implement digital one-shot read logic here
}

void DigitalReader::manualReadOneShot(double &returnedValue)
{
    // Implement digital one-shot read logic and set the returnedValue here
}

void DigitalReader::manualReadPolling()
{
    // Implement digital polling read logic here
    // You can use the provided functions like onPollingTimerTimeOut, onKeyboardHit, etc.
    // to manage the polling process
}
