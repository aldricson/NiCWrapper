#ifndef cPosixFileHelper_h
#define cPosixFileHelper_h

#include <fcntl.h>  // for open
#include <unistd.h> // for close
#include <string>


static inline bool isFileOk(std::string fileName)
{
    int fd = open(fileName.c_str(), O_RDONLY);
    if (fd == -1) 
    {
       // Failed to open the file
       return false;
    }
   else
   {
     close(fd);
     return true;
   }
}

static inline bool createEmptyFile(const std::string &fileName)
{
   // Open a new file with write permission, and create it if it doesn't exist
   int fd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

   if (fd == -1)
   {
     // Failed to create the file
     return false;
   }
   else
   {
     // Close the file descriptor, effectively creating an empty file
     close(fd);
     return true;
   }
}
#endif