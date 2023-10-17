#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <string>
#include <map>
#include <vector>
#include <cstdlib> // for std::strtol
#include <cerrno>  // for errno
#include <cstring> 
#include <string.h> 
#include <iostream>

#include "ini.h"
#include "cPosixFileHelper.h"


class IniParser {
public:
    IniParser();
    ~IniParser();

    std::string currentFilename; 

    
   
    int  readInteger  (std::string section, std::string key, int defaultValue,const std::string& currentFilename);
    bool writeInteger (std::string section, std::string key, int value, const std::string& currentFilename);
    double readDouble(std::string section, std::string key, double defaultValue, const std::string& currentFilename);
    bool writeDouble(std::string section, std::string key, double value, const std::string& currentFilename);
    std::string readString(std::string section, std::string key, const std::string& defaultValue, const std::string& currentFilename);
    bool writeString(std::string section, std::string key, const std::string& value, std::string currentFilename);
    unsigned int readUnsignedInteger(std::string section, std::string key, unsigned int defaultValue, const std::string& currentFilename);
    bool writeUnsignedInteger(std::string section, std::string key, unsigned int value, const std::string& currentFilename);
    bool readBoolean(std::string section, std::string key, bool defaultValue, const std::string& currentFilename);
    bool writeBoolean(std::string section, std::string key, bool value, const std::string& currentFilename);


private:

};

#endif