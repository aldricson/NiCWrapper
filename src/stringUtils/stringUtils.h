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



// Function to center align in text menus
static inline std::string centerAlignString(const std::string& str, unsigned int nbChars) {
    unsigned int totalSpaces = nbChars - str.length();
    unsigned int spacesBefore = totalSpaces / 2;
    unsigned int spacesAfter = totalSpaces - spacesBefore;
    std::string spacesBeforeStr(spacesBefore, ' ');
    std::string spacesAfterStr(spacesAfter, ' ');

    // Construct the result string
    std::string result =  "░" + spacesBeforeStr + str + spacesAfterStr + "░" ;

    return result;
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
    //spacer
    std::string spacer = centerAlignString(" ", maxLength);

    //1) Top line
    std::cout <<  line <<  std::endl;
    //2) spacer
    std::cout << spacer.c_str() << std::endl;
    //3) title 
    std::cout << centerAlignString(title, maxLength)<< std::endl;
    //4) spacer
    std::cout << spacer.c_str() << std::endl;
    //5) line ... Title zone done
    std::cout << line.c_str() << std::endl;
    // Output each centered option
    for (auto option : options) {
        // Append white spaces to make the option string length equal to maxLength
        option.append(maxLength - option.length(), ' ');
        std::cout << centerAlignString(option, maxLength) << std::endl;
    }

    // Bottom of the "table"
    std::cout << spacer.c_str() << std::endl;
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

static inline void clearConsole()
{
    // ANSI escape sequence to clear screen for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
}

// Function to draw a cell with a given size and caption
static inline std::string drawCell(int size, std::string aCaption) {
    
    std::string result;
    
    std::string line;
    // the top and bottom border of the cell
    for (int i = 0; i < size+2; ++i) {
        line += "░";
    }
    line += "\n";
    std::string spacer  = centerAlignString(" ", size)+"\n";
    std::string _caption = centerAlignString(aCaption,size)+"\n"; 
    result = line+spacer+_caption+spacer+line;
    return result;
}

static inline std::string concatenateRow(const std::vector<std::string>& row) {
    std::vector<std::vector<std::string>> linesInCells;

    // Split each cell into lines
    for (const auto& cell : row) {
        std::vector<std::string> lines;
        std::istringstream stream(cell);
        std::string line;
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }
        linesInCells.push_back(lines);
    }

    std::string result;

    // Find the maximum number of lines in any cell
    size_t maxLines = 0;
    for (const auto& lines : linesInCells) {
        maxLines = std::max(maxLines, lines.size());
    }

    // Concatenate lines from each cell
    for (size_t i = 0; i < maxLines; ++i) {
        std::string concatenatedLine;
        for (const auto& lines : linesInCells) {
            if (i < lines.size()) {
                concatenatedLine += lines[i];
            } else {
                // Fill with spaces if this cell has fewer lines
                concatenatedLine += std::string(lines[0].size(), ' ');
            }
        }
        result += concatenatedLine + "\n";
    }

    return result;
}




#endif