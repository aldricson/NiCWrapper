#include "udpBroadcast.h"
#include <mutex>

// Mutex for thread-safe function execution
std::mutex broadcastMutex;

void broadCastStr(const std::string& message) {
    // Lock the mutex for the duration of this function
    std::lock_guard<std::mutex> lock(broadcastMutex);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(std::strerror(errno)));
    }

    // Enable broadcast
    int broadcast = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to set socket option: " + std::string(std::strerror(errno)));
    }

    // Set up destination address
    struct sockaddr_in broadcastAddr;
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadcastAddr.sin_port = htons(8222);

    // Send the broadcast
    if (sendto(sockfd, message.c_str(), message.size(), 0,
               reinterpret_cast<struct sockaddr *>(&broadcastAddr),
               sizeof(broadcastAddr)) < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to send broadcast: " + std::string(std::strerror(errno)));
    }

    close(sockfd);
}
