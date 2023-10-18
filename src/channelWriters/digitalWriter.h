#ifndef DIGITALWRITER_H
#define DIGITALWRITER_H

#include "baseWriter.h"
#include "../NiWrappers/QNiDaqWrapper.h"
#include "../NiWrappers/QNiSysConfigWrapper.h"

class DigitalWriter : public BaseWriter {
public:
    DigitalWriter(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance,
                   std::shared_ptr<QNiDaqWrapper> aDaqMxInstance);
    
private:
    // Private members can be added here.
};

#endif // DIGITALWRITER_H
