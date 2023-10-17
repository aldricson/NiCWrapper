#include "iniParser.h"


// IniParser Implementation
IniParser::IniParser()
{

}   

IniParser::~IniParser()
{

}

int IniParser::readInteger(std::string section, std::string key, int defaultValue, const std::string &currentFilename)
{
    // Check if the file exists
    if (isFileOk(currentFilename))
    {   
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the file
        readOk = file.read(ini);
        if (readOk)
        {
            // Get the value from the structure
            std::string& value = ini[section][key];
            if (value.empty())
            {
                // No value found
                return defaultValue;
            }
            else
            {
                // Value found, return it
                try 
                {
                        return std::stoi(value);
                }
                catch (const std::invalid_argument& e) 
                {
                    return defaultValue;
                }
            }
        }
        else
        {
            // Reading failed
            return defaultValue;
        }
    }
    else
    {
        // File does not exist, return default value
        if(!writeInteger(section,key,defaultValue,currentFilename))
        {
            std::cout<<"impossible to generate default value:"<<section<<" "<<key<<" = "<<defaultValue<<std::endl;
        }
        return defaultValue;
    }
}

bool IniParser::writeInteger(std::string section, std::string key, int value, const std::string &currentFilename)
{
 // Check if the file exists
    if (isFileOk(currentFilename))
    {
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the existing file
        readOk = file.read(ini);
        if (readOk)
        {
           // Update the fields in the structure
           ini[section][key] = std::to_string(value);
           
           // Update the file; check for write success
           if (file.write(ini)) 
           {
               //SUCCESS
               return true;
           } 
           else 
           {
               return false; // File write failed
           }
        }
        else
        {
            return false; // File read failed
        }
    }
    else
    {
        // The file does not exist; create an empty one
        if (createEmptyFile(currentFilename))
        {
            // Now that there's a file, retry writing the value
            return writeInteger(section, key, value, currentFilename);
        }
        else
        {
            return false; // File creation failed
        }
    }
}

double IniParser::readDouble(std::string section, std::string key, double defaultValue, const std::string &currentFilename)
{
    // Check if the file exists
    if (isFileOk(currentFilename))
    {   
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the file
        readOk = file.read(ini);
        if (readOk)
        {
            // Get the value from the structure
            std::string& value = ini[section][key];
            if (value.empty())
            {
                // No value found
                return defaultValue;
            }
            else
            {
                // Value found, return it
                try 
                {
                   return std::stod(value);
                }  
                catch 
                (const std::invalid_argument& e) 
                {
                  return defaultValue;
                }
            }
        }
        else
        {
            // Reading failed
            return defaultValue;
        }
    }
    else
    {
        // File does not exist, return default value
        if(!writeDouble(section,key,defaultValue,currentFilename))
        {
            std::cout<<"impossible to generate default value:"<<section<<" "<<key<<" = "<<defaultValue<<std::endl;
        }
        return defaultValue;
    }
}

bool IniParser::writeDouble(std::string section, std::string key, double value, const std::string &currentFilename)
{
 // Check if the file exists
    if (isFileOk(currentFilename))
    {
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the existing file
        readOk = file.read(ini);
        if (readOk)
        {
           // Update the fields in the structure
           ini[section][key] = std::to_string(value);
           
           // Update the file; check for write success
           if (file.write(ini)) 
           {
               //SUCCESS
               return true;
           } 
           else 
           {
               return false; // File write failed
           }
        }
        else
        {
            return false; // File read failed
        }
    }
    else
    {
        // The file does not exist; create an empty one
        if (createEmptyFile(currentFilename))
        {
            // Now that there's a file, retry writing the value
            return writeDouble(section, key, value, currentFilename);
        }
        else
        {
            return false; // File creation failed
        }
    }
}

std::string IniParser::readString(std::string section, std::string key, const std::string &defaultValue, const std::string &currentFilename)
{
 // Check if the file exists
    if (isFileOk(currentFilename))
    {   
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the file
        readOk = file.read(ini);
        if (readOk)
        {
            // Get the value from the structure
            std::string& value = ini[section][key];
            if (value.empty())
            {
                // No value found
                return defaultValue;
            }
            else
            {
                // Value found, return it
                return value;
            }
        }
        else
        {
            // Reading failed
            return defaultValue;
        }
    }
    else
    {
        // File does not exist, return default value
        if(!writeString(section,key,defaultValue,currentFilename))
        {
            std::cout<<"impossible to generate default value:"<<section<<" "<<key<<" = "<<defaultValue<<std::endl;
        }
        return defaultValue;
    }
}

bool IniParser::writeString(std::string section, std::string key, const std::string &value, std::string currentFilename)
{
      // Check if the file exists
    if (isFileOk(currentFilename))
    {
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the existing file
        readOk = file.read(ini);
        if (readOk)
        {
           // Update the fields in the structure
           ini[section][key] = value;
           
           // Update the file; check for write success
           if (file.write(ini)) 
           {
               //SUCCESS
               return true;
           } 
           else 
           {
               return false; // File write failed
           }
        }
        else
        {
            return false; // File read failed
        }
    }
    else
    {
        // The file does not exist; create an empty one
        if (createEmptyFile(currentFilename))
        {
            // Now that there's a file, retry writing the value
            return writeString(section, key, value, currentFilename);
        }
        else
        {
            return false; // File creation failed
        }
    }
}

unsigned int IniParser::readUnsignedInteger(std::string section, std::string key, unsigned int defaultValue, const std::string &currentFilename)
{
 // Check if the file exists
    if (isFileOk(currentFilename))
    {   
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the file
        readOk = file.read(ini);
        if (readOk)
        {
            // Get the value from the structure
            std::string& value = ini[section][key];
            try 
            {
                  return static_cast<unsigned int>(std::stoul(value));
            } 
            catch (const std::invalid_argument& e)
            {
               return defaultValue;
            } 
            catch (const std::out_of_range& e) 
            {
                return defaultValue;
            }
        }
        else
        {
            // Reading failed
            return defaultValue;
        }
    }
    else
    {
        if(!writeUnsignedInteger(section,key,defaultValue,currentFilename))
        {
            std::cout<<"impossible to generate default value:"<<section<<" "<<key<<" = "<<defaultValue<<std::endl;
        }
        return defaultValue;
    }
}

bool IniParser::writeUnsignedInteger(std::string section, std::string key, unsigned int value, const std::string &currentFilename)
{
    // Check if the file exists
    if (isFileOk(currentFilename))
    {
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the existing file
        readOk = file.read(ini);
        if (readOk)
        {
           // Update the fields in the structure
           ini[section][key] = std::to_string(value);
           
           // Update the file; check for write success
           if (file.write(ini)) 
           {
               //SUCCESS
               return true;
           } 
           else 
           {
               return false; // File write failed
           }
        }
        else
        {
            return false; // File read failed
        }
    }
    else
    {
        // The file does not exist; create an empty one
        if (createEmptyFile(currentFilename))
        {
            // Now that there's a file, retry writing the value
            return writeUnsignedInteger(section, key, value, currentFilename);
        }
        else
        {
            return false; // File creation failed
        }
    }
}

bool IniParser::readBoolean(std::string section, std::string key, bool defaultValue, const std::string &currentFilename)
{
 // Check if the file exists
    if (isFileOk(currentFilename))
    {   
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the file
        readOk = file.read(ini);
        if (readOk)
        {
            // Get the value from the structure
            std::string& value = ini[section][key];
            if (value.empty())
            {
                // No value found
                return defaultValue;
            }
            else
            {
                // Value found, return it
               std::string lowerValue = value;
               std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
               if (lowerValue == "true" || lowerValue == "1") 
               {
                  return true;
               }
               else if 
               (lowerValue == "false" || lowerValue == "0") 
               {
                  return false;
               } 
               else 
               {
                  return defaultValue;
               }
            }
        }
        else
        {
            // Reading failed
            return defaultValue;
        }
    }
    else
    {
        // File does not exist, return default value
        if(!writeBoolean(section,key,defaultValue,currentFilename))
        {
            std::cout<<"impossible to generate default value:"<<section<<" "<<key<<" = "<<defaultValue<<std::endl;
        }
        return defaultValue;
    }
}

bool IniParser::writeBoolean(std::string section, std::string key, bool value, const std::string &currentFilename)
{
 // Check if the file exists
    if (isFileOk(currentFilename))
    {
        // Create file object
        mINI::INIFile file(currentFilename);
        // Create a structure for handling data
        mINI::INIStructure ini;
        bool readOk = false;
        
        // Try to read the existing file
        readOk = file.read(ini);
        if (readOk)
        {
           // Update the fields in the structure
           std::string str;
           value ? str="true" : str="false";
           ini[section][key] = str;
           
           // Update the file; check for write success
           if (file.write(ini)) 
           {
               //SUCCESS
               return true;
           } 
           else 
           {
               return false; // File write failed
           }
        }
        else
        {
            return false; // File read failed
        }
    }
    else
    {
        // The file does not exist; create an empty one
        if (createEmptyFile(currentFilename))
        {
            // Now that there's a file, retry writing the value
            return writeBoolean(section, key, value, currentFilename);
        }
        else
        {
            return false; // File creation failed
        }
    }
}















