#ifndef CRIO_DEBUGSERVER_H
#define CRIO_DEBUGSERVER_H

#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <algorithm>


class CrioDebugServer {
public:
    CrioDebugServer(unsigned short port);
    ~CrioDebugServer();

    void startServer();
    void stopServer();
    static void broadcastMessage(const std::string& message);

private:
    unsigned short           port_           ;
    std::atomic<bool>        serverRunning_  ;
    std::vector<std::thread> clientThreads_  ;
    std::mutex               clientMutex_    ;
    std::condition_variable  clientCondition_;

    static std::mutex              broadcastMutex_  ;
    static std::vector<int>        clientSockets_   ;
    static std::queue<std::string> messageQueue_    ;
    static std::condition_variable messageCondition_;
    static std::atomic<bool> shutdownRequested_; // Static member declaration


    void         acceptClients           ()                          ;
    void         handleClient            (int clientSocket)          ;
    static void  signalHandler           (int signum)                ;
    static void  sendMessageToAllClients (const std::string& message);
    void         processMessageQueue     ()                          ;

};

#endif // CRIO_DEBUGSERVER_H
