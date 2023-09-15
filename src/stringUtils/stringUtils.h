#include <iostream>
#include <cstring>
#include <string>

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