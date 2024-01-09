#include "CrioDebugServer.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>  
#include <cerrno> 
#include <signal.h>


std::mutex CrioDebugServer::broadcastMutex_;
std::vector<int> CrioDebugServer::clientSockets_;
std::queue<std::string> CrioDebugServer::messageQueue_;
std::condition_variable CrioDebugServer::messageCondition_;
std::atomic<bool> CrioDebugServer::shutdownRequested_{false};


CrioDebugServer::CrioDebugServer(unsigned short port) : port_(port), serverRunning_(false) 
{
    signal(SIGINT, CrioDebugServer::signalHandler);
}


// Signal handler function
void CrioDebugServer::signalHandler(int signum) 
{
    // Set the shutdown flag
    shutdownRequested_ = true;
}

CrioDebugServer::~CrioDebugServer() {
    stopServer();
}

void CrioDebugServer::startServer() {
    serverRunning_ = true;
    std::thread(&CrioDebugServer::acceptClients, this).detach();
    std::thread(&CrioDebugServer::processMessageQueue, this).detach(); // Start the message processing thread
}


void CrioDebugServer::stopServer() {
    serverRunning_ = false;
    // Close the server socket here if you opened it in startServer
    clientCondition_.notify_all();
    for (auto& th : clientThreads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void CrioDebugServer::acceptClients() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    // Resource cleanup lambda function
    auto cleanup = [&]() { close(serverSocket); };

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cleanup();
        throw std::runtime_error("Failed to set SO_REUSEADDR: " + std::string(strerror(errno)));
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cleanup();
        throw std::runtime_error("Failed to bind to port: " + std::string(strerror(errno)));
    }

    if (listen(serverSocket, 10) < 0) {
        cleanup();
        throw std::runtime_error("Failed to listen on socket: " + std::string(strerror(errno)));
    }

    try {
        while (serverRunning_ && !shutdownRequested_) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket < 0) {
                if (errno == EINTR) continue; // Interrupted system call, continue
                cleanup();
                throw std::runtime_error("Failed to accept client: " + std::string(strerror(errno)));
            }

            std::lock_guard<std::mutex> lock(clientMutex_);
            clientThreads_.push_back(std::thread(&CrioDebugServer::handleClient, this, clientSocket));
            {
                std::lock_guard<std::mutex> broadcastLock(broadcastMutex_);
                clientSockets_.push_back(clientSocket);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in acceptClients: " << e.what() << std::endl;
        cleanup();
        // Handle or rethrow exception as needed
    }

    cleanup(); // Close server socket when server stops running
}


void CrioDebugServer::handleClient(int clientSocket) {
    std::cout << "Client connected: Socket " << clientSocket << std::endl;

    char buffer[1025]; // Buffer size increased by 1 for null termination
    while (serverRunning_) {
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        ssize_t bytesRead = recv(clientSocket, buffer, 1024, 0); // Limit read to 1024 to leave space for null terminator
        if (bytesRead <= 0) {
            // If no bytes are read, it could mean the client disconnected or there was an error
            if (bytesRead == 0) {
                std::cout << "Client disconnected: Socket " << clientSocket << std::endl;
            } else {
                std::cerr << "Error receiving data: Socket " << clientSocket << std::endl;
            }
            break;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the received data

        // Process the received data here (if necessary)

        // Respond with a standard message for any request
        std::string response = "NACK: debug server does not accept any request";
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    close(clientSocket);
    {
        std::lock_guard<std::mutex> lock(broadcastMutex_);
        auto newEnd = std::remove(clientSockets_.begin(), clientSockets_.end(), clientSocket);
        clientSockets_.erase(newEnd, clientSockets_.end());
    }
}


// void CrioDebugServer::handleClient(int clientSocket) {
//     char buffer[1024];
//     while (serverRunning_) {
//         ssize_t bytesRead = recv(clientSocket, buffer, 1024, 0);
//         if (bytesRead <= 0) {
//             break;
//         }
//         // Respond with a standard message for any request
//         std::string response = "NACK: debug server does not accept any request";
//         send(clientSocket, response.c_str(), response.size(), 0);
//     }
//     close(clientSocket);
//     {
//         std::lock_guard<std::mutex> lock(broadcastMutex_);
//         auto newEnd = std::remove(clientSockets_.begin(), clientSockets_.end(), clientSocket);
//         clientSockets_.erase(newEnd, clientSockets_.end());
//     }
// }

void CrioDebugServer::broadcastMessage(const std::string& message) 
{
    std::lock_guard<std::mutex> lock(broadcastMutex_);
    messageQueue_.push(message);
    messageCondition_.notify_one();
}

void CrioDebugServer::sendMessageToAllClients(const std::string& message) {
    std::lock_guard<std::mutex> lock(broadcastMutex_);
    for (int socket : clientSockets_) {
        send(socket, message.c_str(), message.size(), 0);
    }
}

void CrioDebugServer::processMessageQueue() {
    while (serverRunning_) {
        std::unique_lock<std::mutex> lock(broadcastMutex_);
        messageCondition_.wait(lock, [this] { return !messageQueue_.empty() || !serverRunning_; });

        while (!messageQueue_.empty()) 
        {
            std::string message = messageQueue_.front();
            messageQueue_.pop();
            lock.unlock(); // Unlock while sending messages
            sendMessageToAllClients(message);
            lock.lock(); // Re-lock to check the queue
        }
    }
}
