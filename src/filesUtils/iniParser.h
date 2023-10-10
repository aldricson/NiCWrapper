#ifndef INIPARSER_H
#define INIPARSER_H

#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

struct Comment {
    std::string text;
};

struct Section {
    std::string name;
};

struct KeyValuePair {
    std::string key;
    std::string value;
};

using Line = std::variant<Comment, Section, KeyValuePair>;

class IniParser {
public:
    // Constructor that takes the filename to parse.
    IniParser(const std::string& filename);

    // Function to re-parse the INI file. Useful if the file has changed.
    void reload();

    // Utility functions for fetching and converting data.
    unsigned int readUnsignedInteger(const std::string& section, const std::string& key);
    int          readInteger        (const std::string& section, const std::string& key);
    double       readDouble         (const std::string& section, const std::string& key);
    std::string  readString         (const std::string& section, const std::string& key);
    std::vector<std::string> readStringList(const std::string& section, const std::string& keyPrefix, unsigned int count);

    // Utility functions for writing data.
    void writeUnsignedInteger(const std::string& section, const std::string& key, unsigned int value);
    void writeInteger        (const std::string& section, const std::string& key, int value);
    void writeDouble         (const std::string& section, const std::string& key, double value);
    void writeString         (const std::string& section, const std::string& key, const std::string& value);
    void writeStringList     (const std::string& section, const std::string& keyPrefix, const std::vector<std::string>& values);
    void insertComment       (const std::string& aComment, size_t index);
    void appendComment       (const std::string& aComment);
 

    template <typename Func>
    auto findValue(const std::string& section, const std::string& key, Func&& convertFunc) -> decltype(convertFunc(std::string{}));
    void writeKeyValuePair(const std::string& section, const std::string& key, const std::string& value);


    // Function to save the current data back to the INI file.
    void save();

    // Templated utility function to read an enum.
    template <typename EnumType>
    EnumType readEnum(const std::string& section, const std::string& key);

private:
    // Member variable to hold the parsed data.
    std::vector<Line> lines;
    // Member variable to hold the filename.
    std::string filename;
    // Internal function to actually parse the INI file.
    void parse();

};

#endif // INIPARSER_H
