#include <iostream>
#include <cstring>
#include <string>
#include <algorithm> // for std::transform

static inline std::string removeSpacesFromCharStar(const char* str) {
    int length = strlen(str);
    std::string result;

    for (int i = 0; i < length; i++) {
        if (str[i] != ' ') {
            result += str[i];
        }
    }

    return result;
}



// Function to convert a string to lowercase
static inline std::string toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}