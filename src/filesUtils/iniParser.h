#ifndef INIPARSER_H
#define INIPARSER_H

#include <fstream>
#include <iostream>
#include <string>
#include <functional>

static inline void IniParser(const std::string& filename, std::function<void(const std::string&, const std::string&, const std::string&)> callback) {
    std::ifstream ini(filename);
    if (!ini.is_open()) {
        fprintf(stderr, "Cannot open %s\n", filename.c_str());
        return;
    }

    std::string line;
    std::string section;
    while (std::getline(ini, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        if (line[0] == '[' && line[line.size() - 1] == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        size_t pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        callback(section, key, value);
    }

    ini.close();
}

#endif