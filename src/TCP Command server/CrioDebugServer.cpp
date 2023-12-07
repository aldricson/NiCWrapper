#include "CrioDebugServer.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

std::mutex CrioDebugServer::broadcastMutex_;
std::vector<int> CrioDebugServer::clientSockets_;
std::queue<std::string> CrioDebugServer::messageQueue_;
std::condition_variable CrioDebugServer::messageCondition_;

CrioDebugServer::CrioDebugServer(unsigned short port) : port_(port), serverRunning_(false) {
    //
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


void CrioDebugServer::acceptClients() 
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 10);

    while (serverRunning_) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket > 0) {
            //std::cout << "New client on debug server" << std::endl;
            std::lock_guard<std::mutex> lock(clientMutex_);
            clientThreads_.push_back(std::thread(&CrioDebugServer::handleClient, this, clientSocket));
            {
                std::lock_guard<std::mutex> broadcastLock(broadcastMutex_);
                clientSockets_.push_back(clientSocket);
            }
        }
    }

    close(serverSocket);
}

void CrioDebugServer::handleClient(int clientSocket) {
    char buffer[1024];
    while (serverRunning_) {
        ssize_t bytesRead = recv(clientSocket, buffer, 1024, 0);
        if (bytesRead <= 0) {
            break;
        }
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
