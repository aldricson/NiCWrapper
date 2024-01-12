#include "CrioSSLDebugServer.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>  
#include <cerrno> 
#include <signal.h>
#include <fstream>


std::mutex CrioSSLDebugServer::broadcastMutex_;
std::mutex CrioSSLDebugServer::clientSslMapMutex_;
std::vector<int> CrioSSLDebugServer::clientSockets_;
std::queue<std::string> CrioSSLDebugServer::messageQueue_;
std::condition_variable CrioSSLDebugServer::messageCondition_;
std::atomic<bool> CrioSSLDebugServer::shutdownRequested_{false};
std::map<int, SSL*> CrioSSLDebugServer::clientSslMap_;


CrioSSLDebugServer::CrioSSLDebugServer(unsigned short port) : port_(port), serverRunning_(false) 
{
    initializeSSLContext();
    signal(SIGINT, CrioSSLDebugServer::signalHandler);
}


// Signal handler function
void CrioSSLDebugServer::signalHandler(int signum) 
{
    // Set the shutdown flag
    shutdownRequested_ = true;
}

CrioSSLDebugServer::~CrioSSLDebugServer() {
    stopServer();
}

void CrioSSLDebugServer::startServer() {
    serverRunning_ = true;
    std::thread(&CrioSSLDebugServer::acceptClients, this).detach();
    std::thread(&CrioSSLDebugServer::processMessageQueue, this).detach(); // Start the message processing thread
}


void CrioSSLDebugServer::stopServer() {
    serverRunning_ = false;
    // Close the server socket here if you opened it in startServer
    clientCondition_.notify_all();
    for (auto& th : clientThreads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void CrioSSLDebugServer::initializeSSLContext()
{
     SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    sslContext_ = SSL_CTX_new(SSLv23_server_method());

    if (!sslContext_) {
        logSslErrors("Failed to create SSL context");
        throw std::runtime_error("Failed to create SSL context");
    }

    // Check if certificate file exists
    std::ifstream certFileStream(certFile.c_str());
    if (!certFileStream.good()) 
    {
        std::cerr << "Certificate file " << certFile << " does not exist." << std::endl;
        SSL_CTX_free(sslContext_);
        throw std::runtime_error("Certificate file does not exist");
    }

    // Now use the certificate
    if (SSL_CTX_use_certificate_file(sslContext_, certFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
        logSslErrors("Failed to load certificate");
        SSL_CTX_free(sslContext_);
        throw std::runtime_error("Failed to load certificate");
    }

    if (SSL_CTX_use_PrivateKey_file(sslContext_, keyFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
        logSslErrors("Failed to load private key");
        SSL_CTX_free(sslContext_);
        throw std::runtime_error("Failed to load private key");
    }
}

void CrioSSLDebugServer::acceptClients()
{
    // Create a server socket using TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    // Lambda function for cleanup
    auto cleanup = [&]() { close(serverSocket); };

    // Set socket options for reuse
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cleanup();
        throw std::runtime_error("Failed to set SO_REUSEADDR: " + std::string(strerror(errno)));
    }

    // Configure server address structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the server socket to the specified port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cleanup();
        throw std::runtime_error("Failed to bind to port: " + std::string(strerror(errno)));
    }

    // Start listening on the server socket
    if (listen(serverSocket, 10) < 0) {
        cleanup();
        throw std::runtime_error("Failed to listen on socket: " + std::string(strerror(errno)));
    }

    // Main loop to accept incoming connections
    try {
        while (serverRunning_ && !shutdownRequested_) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket < 0) {
                if (errno == EINTR) continue; // If interrupted by signal, just continue
                cleanup();
                throw std::runtime_error("Failed to accept client: " + std::string(strerror(errno)));
            }

            // Create a new SSL object for the connection
            SSL *ssl = SSL_new(sslContext_);
            if (!ssl) {
                close(clientSocket); // Close the socket if SSL creation failed
                continue;
            }

            // Set the file descriptor for the SSL object
            SSL_set_fd(ssl, clientSocket);

            // Perform SSL handshake
            if (SSL_accept(ssl) <= 0) {
                SSL_free(ssl); // Free SSL object if handshake failed
                close(clientSocket); // Close the socket
                continue;
            }

            // Lock the mutex to safely add the client thread
            std::lock_guard<std::mutex> lock(clientMutex_);
            // Start a new thread to handle the client's SSL connection
            clientThreads_.push_back(std::thread(&CrioSSLDebugServer::handleClient, this, clientSocket, ssl));

            // Store client socket and SSL object in maps for future reference
            {
                std::lock_guard<std::mutex> broadcastLock(broadcastMutex_);
                clientSockets_.push_back(clientSocket);
                std::lock_guard<std::mutex> lock(clientSslMapMutex_);
                clientSslMap_[clientSocket] = ssl;
            }
        }
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Exception in acceptClients: " << e.what() << std::endl;
        cleanup();
        // Optionally rethrow the exception or handle as needed
    }

    cleanup(); // Cleanup the server socket after loop ends
}

void CrioSSLDebugServer::handleClient(int clientSocket, SSL* ssl) 
{
    std::cout << "SSL Client connected: Socket " << clientSocket << std::endl;

    char buffer[1025]; // Buffer size increased by 1 for null termination
    while (serverRunning_) {
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Use SSL_read instead of recv for SSL communication
        ssize_t bytesRead = SSL_read(ssl, buffer, 1024); // Limit read to 1024 to leave space for null terminator
        if (bytesRead <= 0) {
            int sslError = SSL_get_error(ssl, bytesRead);
            if (sslError == SSL_ERROR_ZERO_RETURN || sslError == SSL_ERROR_SYSCALL) 
            {
                std::cout << "SSL Client disconnected: Socket " << clientSocket << std::endl;
            } 
            else 
            {
                std::cerr << "SSL Error receiving data: Socket " << clientSocket << " Error: " << sslError << std::endl;
            }
            break; // Break on disconnect or error
        }

        buffer[bytesRead] = '\0'; // Null-terminate the string

        // Process the received data here (if necessary)

        // Respond with a standard message for any request
        std::string response = "NACK: SSL debug server does not accept any request";
        // Use SSL_write instead of send for SSL communication
        SSL_write(ssl, response.c_str(), response.size());
    }

    // Properly close the SSL connection and free resources
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(clientSocket); // Close the socket

    // Update clientSockets_ list
    {
        std::lock_guard<std::mutex> lock(broadcastMutex_);
        auto newEnd = std::remove(clientSockets_.begin(), clientSockets_.end(), clientSocket);
        clientSockets_.erase(newEnd, clientSockets_.end());
        std::lock_guard<std::mutex> lock2(clientSslMapMutex_);
        clientSslMap_.erase(clientSocket);
    }
}




void CrioSSLDebugServer::broadcastMessage(const std::string& message) 
{
    std::lock_guard<std::mutex> lock(broadcastMutex_);
    messageQueue_.push(message);
    messageCondition_.notify_one();
}

void CrioSSLDebugServer::sendMessageToAllClients(const std::string& message) {
    std::lock_guard<std::mutex> lock(broadcastMutex_);
    std::lock_guard<std::mutex> lock2(clientSslMapMutex_);
    for (int socket : clientSockets_) 
    {
        SSL* ssl = clientSslMap_[socket]; // Retrieve the SSL object for the client socket
        if (ssl) {
            SSL_write(ssl, message.c_str(), message.size()); // Use SSL_write for SSL communication
        }
    }
}

void CrioSSLDebugServer::processMessageQueue() {
    while (serverRunning_) {
        std::unique_lock<std::mutex> lock(broadcastMutex_);
        messageCondition_.wait(lock, [this] { return !messageQueue_.empty() || !serverRunning_; });

        while (!messageQueue_.empty()) {
            std::string message = messageQueue_.front();
            messageQueue_.pop();
            lock.unlock(); // Unlock while sending messages
            sendMessageToAllClients(message); // Send message using SSL
            lock.lock(); // Re-lock to check the queue
        }
    }
}

void CrioSSLDebugServer::logSslErrors(const std::string &message)
{
        std::cerr << message << std::endl;
    unsigned long errCode;
    while((errCode = ERR_get_error())) {
        char *err = ERR_error_string(errCode, NULL);
        std::cerr << "OpenSSL error: " << err << std::endl;
    }
}

