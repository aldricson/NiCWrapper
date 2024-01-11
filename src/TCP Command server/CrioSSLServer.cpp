#include "CrioSSLServer.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


CrioSSLServer::CrioSSLServer(unsigned short port,
                            std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper,
                            std::shared_ptr<QNiDaqWrapper>       aDaqWrapper,
                            std::shared_ptr<AnalogicReader>      anAnalogicReader,
                            std::shared_ptr<DigitalReader>       aDigitalReader,
                            std::shared_ptr<NItoModbusBridge>    aBridge) : 
                            port_(port),
                            m_cfgWrapper(aConfigWrapper),
                            m_daqWrapper(aDaqWrapper),
                            m_analogicReader(anAnalogicReader),
                            m_digitalReader(aDigitalReader),
                            m_bridge(aBridge),
                            serverRunning_(false) 
{
    initializeSSLContext();
}

CrioSSLServer::~CrioSSLServer() 
{
    stopServer();
    cleanupSSLContext(); // Ensure SSL context and resources are cleaned up
}

void CrioSSLServer::startServer() {
    serverRunning_ = true;
    std::thread(&CrioSSLServer::acceptClients, this).detach();
}

void CrioSSLServer::stopServer() {
    serverRunning_ = false;
    clientCondition_.notify_all();
    for (auto& th : clientThreads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void CrioSSLServer::initializeSSLContext() 
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

void CrioSSLServer::logSslErrors(const std::string& message) {
    std::cerr << message << std::endl;
    unsigned long errCode;
    while((errCode = ERR_get_error())) {
        char *err = ERR_error_string(errCode, NULL);
        std::cerr << "OpenSSL error: " << err << std::endl;
    }
}

void CrioSSLServer::cleanupSSLContext() {
    // Check if the SSL context exists
    if (sslContext_ != nullptr) {
        // Free the SSL context
        SSL_CTX_free(sslContext_);
        sslContext_ = nullptr; // Set the pointer to nullptr to avoid use after free
    }

    // Clean up OpenSSL error strings and algorithms
    ERR_free_strings();
    EVP_cleanup();
}


void CrioSSLServer::acceptClients() {
    // Create a server socket using TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    // Lambda function for cleanup
    auto cleanup = [&]() { close(serverSocket); };

    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cleanup();
        throw std::runtime_error("Failed to set SO_REUSEADDR: " + std::string(strerror(errno)));
    }

    // Configure server address
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
    while (serverRunning_) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            if (errno == EINTR) {
                continue; // If interrupted by signal, just continue
            } else {
                cleanup();
                throw std::runtime_error("Failed to accept client: " + std::string(strerror(errno)));
            }
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
        clientThreads_.push_back(std::thread(&CrioSSLServer::handleClient, this, clientSocket, ssl));
    }

    // Cleanup the server socket
    cleanup();
}


void CrioSSLServer::handleClient(int clientSocket, SSL* ssl) {
    char buffer[257]; // Buffer size increased by 1 for null termination
    std::string accumulatedData;
    const size_t maxMessageSize = 256;

    while (serverRunning_) {
        memset(buffer, 0, sizeof(buffer));
        // Use SSL_read instead of recv for SSL communication
        ssize_t bytesRead = SSL_read(ssl, buffer, maxMessageSize);
        if (bytesRead <= 0) {
            int sslError = SSL_get_error(ssl, bytesRead);
            if (sslError == SSL_ERROR_ZERO_RETURN || sslError == SSL_ERROR_SYSCALL) {
                std::cout << "SSL Client disconnected: Socket " << clientSocket << std::endl;
            } else {
                std::cerr << "SSL Error receiving data: Socket " << clientSocket << " Error: " << sslError << std::endl;
            }
            break; // Break on disconnect or error
        }

        buffer[bytesRead] = '\0'; // Null-terminate the string
        accumulatedData += buffer;

        if (accumulatedData.length() > maxMessageSize) {
            std::string response = "NACK: command rejected";
            // Use SSL_write instead of send for SSL communication
            SSL_write(ssl, response.c_str(), response.size());
            break; // Optionally disconnect the client
        }

        size_t delimiterPos = accumulatedData.find('\n');
        if (delimiterPos != std::string::npos) {
            std::string completeMessage = accumulatedData.substr(0, delimiterPos);
            accumulatedData.erase(0, delimiterPos + 1);

            std::string response = parseRequest(completeMessage);
            // Use SSL_write to send response back to the client
            SSL_write(ssl, response.c_str(), response.size());
        }
    }

    // Properly close the SSL connection and free resources
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(clientSocket); // Close the socket
}


std::string CrioSSLServer::parseRequest(const std::string& request) 
{
    std::string requestCopy = request;
    std::vector<std::string> tokens;
    bool ok; 
    tokenize(requestCopy, tokens, ok);
    if (tokens.size() > 3 || !ok) 
    {
        return "NACK: Invalid command format";
    }

    if (checkForReadCommand(tokens[0],"readCurrent"))
    {
        // Ensure there are enough tokens for a valid command
        if (tokens.size() < 3) 
        {
            return "NACK: Invalid command format";
        }
        try
        {
            std::string moduleAlias = tokens[1];
            bool ok;
            unsigned int channelIndex = strToUnsignedInt(tokens[2],ok);
            //double result = m_daqWrapper->readCurrent(deviceModule,channelIndex,50,true); 
            double result;
            m_analogicReader->manualReadOneShot(moduleAlias,channelIndex,result);
            return  std::to_string(result); 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return std::string("NACK:") + e.what();
        }   
    }

    else if (checkForReadCommand(tokens[0],"readVoltage"))
    {
        // Ensure there are enough tokens for a valid command
        if (tokens.size() < 3) 
        {
            return "NACK: Invalid command format";
        }
        try
        {
            std::string moduleAlias = tokens[1];
            bool ok;
            unsigned int channelIndex = strToUnsignedInt(tokens[2],ok);
            if (ok)
            {
                //std::cout<<"received readVoltage: "<<moduleAlias<<" channel index: "<<tokens[2]<<std::endl;

                try 
                {
                    NIDeviceModule *deviceModule = m_cfgWrapper->getModuleByAlias(moduleAlias);
                    if (deviceModule != nullptr) 
                    {
                        double result = m_daqWrapper->readVoltage(deviceModule,channelIndex,50);
                        return std::to_string(result);
                    } 
                    else 
                    {
                        return std::string("NACK: deviceModule is nullptr");
                    }
                }
                catch (const std::bad_alloc& e)
                {
                    // Handle memory allocation failure
                    std::cerr << "Memory allocation failed: " << e.what() << '\n';
                    return std::string("NACK: Memory allocation failed") + e.what();
                }
                
            }
            else
            {
                return ("NACK:Impossible to convert "+tokens[2]+"to unsignedInt");
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return std::string("NACK:") + e.what();
        }      
    }

    else if (checkForReadCommand(tokens[0],"startModbusSimulation"))
    {
        // Ensure there are enough tokens for a valid command
        if (tokens.size() != 1) 
        {
            return "NACK: Invalid command format";
        }
        
        //broadCastStr("crio debug:\nstartModbusSimulation detected\nin std::string CrioSSLServer::parseRequest(const std::string& request)\n"); 
        try
        {
            if (m_bridge->getSimulateTimer()->isActive()) 
            {
                //broadCastStr("crio debug:\nsimulation timer already active\nin std::string CrioSSLServer::parseRequest(const std::string& request)\n"); 
                //simulation already avtive
                return "ACK";
            }
            if (m_bridge->startModbusSimulation()) 
            {
               return ("ACK");
            }
            else
            { 
              return ("NACK: Impossible to start modbus simulation");
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            //broadCastStr("crio debug:\n"+ std::string(e.what())+"\nin std::string CrioSSLServer::parseRequest(const std::string& request)\n");
            return std::string("NACK:") + std::string(e.what());
        }
    }

    else if (checkForReadCommand(tokens[0],"stopModbusSimulation"))
    {
        // Ensure there is only one token for a valid command
        if (tokens.size() != 1) 
        {
            return "NACK: Invalid command format";
        }
        
        try
        {
            m_bridge->stopModbusSimulation();
            return ("ACK"); 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return std::string("NACK:") + e.what();
        }
    }

    else if (checkForReadCommand(tokens[0],"startModbusAcquisition"))
    {
        // Ensure there is only one token for a valid command
        if (tokens.size() != 1) 
        {
            return "NACK: Invalid command format";
        }
        
        try
        {
            if (m_bridge->getDataAcquTimer()->isActive()) 
            {
                return "ACK";
            }
            if (m_bridge->startAcquisition()) 
            {
               return ("ACK");
            }
            else
            { 
              return ("NACK: Impossible to start modbus acquisition");
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return std::string("NACK:") + std::string(e.what());
        }
    }

    else if (checkForReadCommand(tokens[0],"stopModbusAcquisition"))
    {
        // Ensure there is only one token for a valid command
        if (tokens.size() != 1) 
        {
            return "NACK: Invalid command format";
        }
        
        try
        {
            m_bridge->stopAcquisition();
            return ("ACK"); 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return std::string("NACK:") + e.what();
        }
    }

    else
    {
        return "unknow command";
    }
}



void CrioSSLServer::tokenize(const std::string& input, std::vector<std::string>& tokens, bool &ok) 
{
    std::string inputCopy = input;
    size_t pos = 0;
    size_t nbToken=0;
    ok = true;
    while ((pos = inputCopy.find(';')) != std::string::npos)
    {
        std::string token = inputCopy.substr(0, pos);
        tokens.push_back(token);
        inputCopy.erase(0, pos + 1);  // Move the position past the semicolon
        if (nbToken>=20)
        {
            ok = false;
            break;
        }
        nbToken++;
    }

    // After the loop, inputCopy may still have some data after the last semicolon, which can be considered as another token.
    if (!inputCopy.empty())
    {
        tokens.push_back(inputCopy);
    }
}

bool CrioSSLServer::checkForReadCommand(const std::string& request, const std::string& command)
{
    std::string mainString      =  request;
    std::string substringToFind = command;
    size_t foundPos = mainString.find(substringToFind);
    return foundPos != std::string::npos;
}


