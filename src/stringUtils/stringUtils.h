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

//function to center align in text menus
static inline std::string centerAlignString(const std::string& str, unsigned int nbChars) {
    unsigned int totalSpaces = nbChars - str.length();
    unsigned int spacesBefore = totalSpaces / 2;
    unsigned int spacesAfter = totalSpaces - spacesBefore;
    std::string spacesBeforeStr(spacesBefore, ' ');
    std::string spacesAfterStr(spacesAfter, ' ');
    return "░" + spacesBeforeStr + str + spacesAfterStr + "░";
}
