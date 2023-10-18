#include "digitalWriter.h"

DigitalWriter::DigitalWriter(std::shared_ptr<QNiSysConfigWrapper> aSysConfigInstance, std::shared_ptr<QNiDaqWrapper> aDaqMxInstance)
    : BaseWriter(aSysConfigInstance, aDaqMxInstance)  // Explicitly call the BaseWriter constructor
{
}
