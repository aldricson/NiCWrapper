#ifndef UDP_BROADCAST_H
#define UDP_BROADCAST_H

#include <string>
#include <stdexcept>
#include <cstring> // for std::strerror
#include <cerrno>  // for errno
#include <sys/socket.h> // for socket functions
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h>  // for inet_addr
#include <unistd.h>  

// Function to broadcast a string over UDP on port 8222
void broadCastStr(const std::string& message);

#endif // UDP_BROADCAST_H
