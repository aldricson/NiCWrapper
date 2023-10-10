#include "iniParser.h"

// Constructor implementation
IniParser::IniParser(const std::string& filename) : filename(filename) {
    parse();
}

// Template function to find a value in the parsed INI data and convert it to the desired type.
// The function takes a section name, a key name, and a lambda function for conversion.
template <typename Func>
auto IniParser::findValue(const std::string& section, const std::string& key, Func&& convertFunc) -> decltype(convertFunc(std::string{})) {
    // Variable to hold the current section name while iterating through the lines.
    std::string sectionName;
    // Boolean flag to indicate whether the key was found.
    bool keyFound = false;
    // Variable to hold the return value. Its type is inferred from the lambda function.
    decltype(convertFunc(std::string{})) returnValue;

    // Iterate through each line in the parsed INI data.
    for (const auto& line : lines) {
        // Use std::visit to apply a lambda function to the variant 'line'.
        std::visit([&](auto&& arg) {
            // Determine the type of the current line (could be Comment, Section, or KeyValuePair).
            using T = std::decay_t<decltype(arg)>;
            // If the line is a section header, update the current section name.
            if constexpr (std::is_same_v<T, Section>) {
                sectionName = arg.name;
            } 
            // If the line is a key-value pair, check if it's the key we're looking for.
            else if constexpr (std::is_same_v<T, KeyValuePair>) {
                if (sectionName == section && arg.key == key) {                
                    // If found, convert the value using the provided lambda function and set the flag.
                    returnValue = convertFunc(arg.value);
                    keyFound = true;
                }
            }
        }, line);
    }
    // If the key was not found in any section, throw a runtime error.
    if (!keyFound) {
        throw std::runtime_error("Key not found");
    }
    // Return the converted value.
    return returnValue;
}



// Helper function to write a key-value pair to the parsed INI data.
void IniParser::writeKeyValuePair(const std::string& section, const std::string& key, const std::string& value) {
    // Variable to hold the current section name while iterating through the lines.
    std::string sectionName;
    // Boolean flag to indicate whether the section was found.
    bool sectionFound = false;

    // Iterate through each line in the parsed INI data.
    for (auto& line : lines) {
        // Use std::visit to apply a lambda function to the variant 'line'.
        std::visit([&](auto&& arg) {
            // Determine the type of the current line (could be Comment, Section, or KeyValuePair).
            using T = std::decay_t<decltype(arg)>;
            
            // If the line is a section header, update the current section name.
            if constexpr (std::is_same_v<T, Section>) {
                sectionName = arg.name;
                // Check if the current section is the one we're looking for.
                if (sectionName == section) {
                    sectionFound = true;
                }
            } 
            // If the line is a key-value pair and we're in the correct section, update the value.
            else if constexpr (std::is_same_v<T, KeyValuePair>) {
                if (sectionFound && arg.key == key) {
                    
                    // Update the value of the key-value pair.
                    arg.value = value;
                    // Exit the lambda function early as the key-value pair has been updated.
                    return;
                }
            }
        }, line);
    }
    // If the section was not found, add it to the end of the lines vector.
    if (!sectionFound) {
        lines.push_back(Section{section});
    }
    // Add the new key-value pair to the end of the lines vector.
    lines.push_back(KeyValuePair{key, value});
}



void IniParser::parse() {
    // Check if the file exists before attempting to open it
    std::ifstream fileCheck(filename);
    if (!fileCheck.is_open()) {
        // File doesn't exist, simply return without doing anything
        return;
    }
    fileCheck.close(); // Close the fileCheck stream

    lines.clear();
    std::ifstream ini(filename);
    if (!ini.is_open()) {
        throw std::runtime_error("Cannot open " + filename);
    }

    std::string line;
    while (std::getline(ini, line)) {
        // Remove leading and trailing whitespace.
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Skip empty lines.
        if (line.empty()) {
            continue;
        }

        // Handle comments.
        if (line[0] == '#' || line[0] == ';') {
            lines.push_back(Comment{line});
            continue;
        }

        // Handle section headers.
        if (line[0] == '[' && line[line.size() - 1] == ']') {
            lines.push_back(Section{line.substr(1, line.size() - 2)});
            continue;
        }

        // Handle key-value pairs.
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            lines.push_back(KeyValuePair{key, value});
        }
    }

    ini.close();
}

// Utility function to read an unsigned integer from the parsed INI data.
unsigned int IniParser::readUnsignedInteger(const std::string& section, const std::string& key) {
    return findValue(section, key, [](const std::string& value) { return std::stoul(value); });
}

// Utility function to read an integer from the parsed INI data.
int IniParser::readInteger(const std::string& section, const std::string& key) {
    return findValue(section, key, [](const std::string& value) { return std::stoi(value); });
}

// Utility function to read a double from the parsed INI data.
double IniParser::readDouble(const std::string& section, const std::string& key) {
    return findValue(section, key, [](const std::string& value) { return std::stod(value); });
}

// Utility function to read a string from the parsed INI data.
std::string IniParser::readString(const std::string& section, const std::string& key) {
    return findValue(section, key, [](const std::string& value) { return value; });
}

// Utility function to read a list of strings from the parsed INI data.
std::vector<std::string> IniParser::readStringList(const std::string& section, const std::string& keyPrefix, unsigned int count) {
    // Vector to store the values read from the INI file.
    std::vector<std::string> values;

    // Variable to hold the current section name while iterating through the lines.
    std::string sectionName;

    // Iterate through each line in the parsed INI data.
    for (const auto& line : lines) {
        std::visit([&](auto&& arg) {
            // Determine the type of the current line (could be Comment, Section, or KeyValuePair).
            using T = std::decay_t<decltype(arg)>;

            // If the line is a section header, update the current section name.
            if constexpr (std::is_same_v<T, Section>) {
                sectionName = arg.name;
            } 
            // If the line is a key-value pair and we're in the correct section, read the value.
            else if constexpr (std::is_same_v<T, KeyValuePair>) {
                if (sectionName == section) {
                    for (unsigned int i = 0; i < count; ++i) {
                        std::string key = keyPrefix + std::to_string(i);
                        if (arg.key == key) {
                            values.push_back(arg.value);
                        }
                    }
                }
            }
        }, line);
    }

    // Check if the expected number of keys were found.
    if (values.size() != count) {
        throw std::runtime_error("Not all keys found");
    }

    // Return the values read from the INI file.
    return values;
}

// Function to re-parse the INI file. Useful if the file has changed.
void IniParser::reload(){
    parse();
}

// Utility function to read an enum from the parsed INI data.
template <typename EnumType>
EnumType IniParser::readEnum(const std::string& section, const std::string& key) {
    return static_cast<EnumType>(findValue(section, key, [](const std::string& value) { return std::stoi(value); }));
}

// Utility function to write an unsigned integer to the parsed INI data.
void IniParser::writeUnsignedInteger(const std::string& section, const std::string& key, unsigned int value) {
    writeKeyValuePair(section, key, std::to_string(value));
}

// Utility function to write an integer to the parsed INI data.
void IniParser::writeInteger(const std::string& section, const std::string& key, int value) {
    writeKeyValuePair(section, key, std::to_string(value));
}

// Utility function to write a double to the parsed INI data.
void IniParser::writeDouble(const std::string& section, const std::string& key, double value) {
    writeKeyValuePair(section, key, std::to_string(value));
}

// Utility function to write a string to the parsed INI data.
void IniParser::writeString(const std::string& section, const std::string& key, const std::string& value) {
    writeKeyValuePair(section, key, value);
}



// Utility function to write a list of strings to the parsed INI data.
void IniParser::writeStringList(const std::string& section, const std::string& keyPrefix, const std::vector<std::string>& values) {
    // Debugging output to show the function has been entered and to display the key prefix.
    std::cout << "enter write string list:" << keyPrefix << std::endl;

    // Debugging output to display all the values in the list.
    for (int i = 0; i < values.size(); ++i) {
        std::cout << values[i] << std::endl;
    }

    // Variable to hold the current section name while iterating through the lines.
    std::string sectionName;

    // Flags to indicate whether the section and keys were found.
    bool sectionFound = false;
    bool keysUpdated = false;

    // Iterate through each line in the parsed INI data.
    for (auto& line : lines) {
        std::visit([&](auto&& arg) {
            // Determine the type of the current line (could be Comment, Section, or KeyValuePair).
            using T = std::decay_t<decltype(arg)>;

            // If the line is a section header, update the current section name.
            if constexpr (std::is_same_v<T, Section>) {
                sectionName = arg.name;
                if (sectionName == section) {
                    sectionFound = true;
                }
            } 
            // If the line is a key-value pair and we're in the correct section, update the value.
            else if constexpr (std::is_same_v<T, KeyValuePair>) {
                if (sectionFound) {
                    for (size_t i = 0; i < values.size(); ++i) {
                        std::string key = keyPrefix + std::to_string(i);
                        if (arg.key == key) {
                            arg.value = values[i];
                            keysUpdated = true;
                        }
                    }
                }
            }
        }, line);
    }

    // If the section was not found, add it and the key-value pairs.
    if (!sectionFound) {
        lines.push_back(Section{section});
        for (size_t i = 0; i < values.size(); ++i) {
            std::string key = keyPrefix + std::to_string(i);
            lines.push_back(KeyValuePair{key, values[i]});
        }
    } 
    // If keys were not updated, it means they don't exist. Add them.
    else if (!keysUpdated) {
        for (size_t i = 0; i < values.size(); ++i) {
            std::string key = keyPrefix + std::to_string(i);
            lines.push_back(KeyValuePair{key, values[i]});
        }
    }
}


// Function to insert a comment at a specific index in the parsed INI data.
void IniParser::insertComment(const std::string& aComment, size_t index) {
    // Check if the index is valid.
    if (index > lines.size()) {
        std::cerr << "Invalid index for inserting comment" << std::endl;
        return;
    }
    // Create a Comment object.
    Comment newComment;
    newComment.text = "# " + aComment;
    // Insert the Comment object at the specified index in the 'lines' vector.
    lines.insert(lines.begin() + index, newComment);
}

void IniParser::appendComment(const std::string& aComment) {
    // Create a Comment object.
    Comment newComment;
    newComment.text = "# " + aComment;

    // Append the Comment object at the end of the 'lines' vector.
    lines.push_back(newComment);
}


// Function to save the current data back to the INI file.
void IniParser::save() {
    std::ofstream ini(filename);
    if (!ini.is_open()) {
        std::cerr << "Cannot open " << filename << " for writing" << std::endl;
        return;
    }

    for (const auto& line : lines) {
        std::visit([&ini](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Comment>) {
                ini << arg.text << "\n";
            } else if constexpr (std::is_same_v<T, Section>) {
                ini << "[" << arg.name << "]\n";
            } else if constexpr (std::is_same_v<T, KeyValuePair>) {
                ini << arg.key << "=" << arg.value << "\n";
            }
        }, line);
    }

    ini.close();
}


