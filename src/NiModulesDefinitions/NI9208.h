#ifndef NI9208_H
#define NI9208_H

#include "NIDeviceModule.h"
#include <vector>
#include <string>
#include <fstream>



class NI9208 : public NIDeviceModule {
private:

public:
    NI9208();

    void initModule()                       override;
    std::vector<std::string> getChanNames() const override;
    void loadConfig()  override;
    void saveConfig()  override;
};

#endif // NI9208_H
