#ifndef STRINGUTILS_H
#define STRINGUTILS_H
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm> // for std::transform
#include <functional>
#include <sstream>

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


static inline void displayMenu(const std::string& title,
                               const std::vector<std::string>& options)
{
        // Find the length of the longest string in options
    unsigned int maxLength = 0;
    for (const auto& option : options) {
        if (option.length() > maxLength) {
            maxLength = option.length();
        }
    }

    // Make sure maxLength is even for center alignment
    if (maxLength % 2 != 0) {
        maxLength++;
    }

    //Give some fresh air to the eyes
    maxLength+=4;

    //Create the top and bottom line
    std::string line;
    for (unsigned int i = 0; i < maxLength+2; ++i) line += "░";
    //
    std::string spacer = centerAlignString(" ", maxLength);

    // Output the centered title
    std::cout << line   << std::endl;
    std::cout << spacer << std::endl;
    std::cout << centerAlignString(title, maxLength) << std::endl;
    std::cout << spacer << std::endl;
    std::cout << line << std::endl;

    // Output each centered option
    for (auto option : options) {
        // Append white spaces to make the option string length equal to maxLength
        option.append(maxLength - option.length(), ' ');
        std::cout << centerAlignString(option, maxLength) << std::endl;
    }

    // Bottom of the "table"
    std::cout << spacer << std::endl;
    std::cout << line << std::endl;

}

static inline void handleUserInput(
        const std::vector<std::string>& options,
        const std::vector<std::function<void()>>& actions,
        const std::function<void()> &retryFunction)
    {
        std::string choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.clear();
        std::cin.ignore();

        unsigned int selectedChoice;
        std::stringstream ss(choice);
        if (ss >> selectedChoice && ss.eof() && selectedChoice < actions.size()) {
            actions[selectedChoice]();
        } 
        else 
        {
            std::cout << "Invalid selection. Try again." << std::endl;
            retryFunction();
        }
    }

static inline void displayMenu(const std::string& title,
                               const std::vector<std::string>& options,
                               const std::vector<std::function<void()>>& actions,
                               const std::function<void()> &retryFunction) 
{
    displayMenu(title,options);
    handleUserInput(options,actions,retryFunction);
}





#endif