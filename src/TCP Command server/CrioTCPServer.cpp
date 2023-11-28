#include "CrioTCPServer.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

CrioTCPServer::CrioTCPServer(unsigned short port,
                            std::shared_ptr<QNiSysConfigWrapper> aConfigWrapper,
                            std::shared_ptr<QNiDaqWrapper>       aDaqWrapper,
                            std::shared_ptr<AnalogicReader>      anAnalogicReader,
                            std::shared_ptr<DigitalReader>       aDigitalReader,
                            std::shared_ptr<DigitalWriter>       aDigitalWriter,
                            std::shared_ptr<NItoModbusBridge>    aBridge) : 
                            port_(port),
                            m_cfgWrapper(aConfigWrapper),
                            m_daqWrapper(aDaqWrapper),
                            m_analogicReader(anAnalogicReader),
                            m_digitalReader(aDigitalReader),
                            m_digitalWriter(aDigitalWriter),
                            m_bridge(aBridge),
                            serverRunning_(false) 
{
    //
}

CrioTCPServer::~CrioTCPServer() {
    stopServer();
}

void CrioTCPServer::startServer() {
    serverRunning_ = true;
    std::thread(&CrioTCPServer::acceptClients, this).detach();
}

void CrioTCPServer::stopServer() {
    serverRunning_ = false;
    clientCondition_.notify_all();
    for (auto& th : clientThreads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void CrioTCPServer::acceptClients() {
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
            std::lock_guard<std::mutex> lock(clientMutex_);
            clientThreads_.push_back(std::thread(&CrioTCPServer::handleClient, this, clientSocket));
        }
    }

    close(serverSocket);
}

void CrioTCPServer::handleClient(int clientSocket) {
    char buffer[1024];
    while (serverRunning_) {
        ssize_t bytesRead = recv(clientSocket, buffer, 1024, 0);
        if (bytesRead <= 0) {
            break;
        }
        std::string request(buffer, bytesRead);
        std::string response = parseRequest(request);
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    close(clientSocket);
}

std::string CrioTCPServer::parseRequest(const std::string& request) 
{
    std::string requestCopy = request;
    std::vector<std::string> tokens; 
    tokenize(requestCopy, tokens);

    if (checkForReadCommand(tokens[0],"readCurrent"))
    {
        try
        {
            std::string moduleAlias = tokens[1];
            bool ok;
            unsigned int channelIndex = strToUnsignedInt(tokens[2],ok);
            NIDeviceModule *deviceModule     = m_cfgWrapper->getModuleByAlias(moduleAlias);
            double result = m_daqWrapper->readCurrent(deviceModule,channelIndex,50,true); 
            return  std::to_string(result); 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return ("NACK");
        }   
    }
    else if (checkForReadCommand(tokens[0],"readVoltage"))
    {
        try
        {
            std::string moduleAlias = tokens[1];
            bool ok;
            unsigned int channelIndex = strToUnsignedInt(tokens[2],ok);
            NIDeviceModule *deviceModule     = m_cfgWrapper->getModuleByAlias(moduleAlias);
            double result = m_daqWrapper->readVoltage(deviceModule,channelIndex,50);
            return std::to_string(result);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return ("NACK");
        }      
    }
    else if (checkForReadCommand(tokens[0],"startModbusSimulation"))
    {
        try
        {
            m_bridge->stopAcquisition();
            m_bridge->startModbusSimulation();
            return ("ACK"); 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return ("NACK");
        }
    }
    else if (checkForReadCommand(tokens[0],"stopModbusSimulation"))
    {
        try
        {
            m_bridge->stopModbusSimulation();
            return ("ACK"); 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return ("NACK");
        }
    }
    else
    {
        return "unknow command";
    }
}


void CrioTCPServer::tokenize(const std::string& input, std::vector<std::string>& tokens) 
{
    std::string inputCopy = input;
    size_t pos = 0;
    while ((pos = inputCopy.find(';')) != std::string::npos)
    {
        std::string token = inputCopy.substr(0, pos);
        tokens.push_back(token);
        inputCopy.erase(0, pos + 1);  // Move the position past the semicolon
    }

    // After the loop, inputCopy may still have some data after the last semicolon, which can be considered as another token.
    if (!inputCopy.empty())
    {
        tokens.push_back(inputCopy);
    }
}

bool CrioTCPServer::checkForReadCommand(const std::string& request, const std::string& command)
{
    std::string mainString      =  request;
    std::string substringToFind = command;
    size_t foundPos = mainString.find(substringToFind);
    return foundPos != std::string::npos;
}



