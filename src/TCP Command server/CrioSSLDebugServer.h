#ifndef CrioSSLDebugServer_H
#define CrioSSLDebugServer_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <algorithm>
#include <map>


class CrioSSLDebugServer {
public:
    CrioSSLDebugServer(unsigned short port);
    ~CrioSSLDebugServer();

    void startServer();
    void stopServer();
    static void broadcastMessage(const std::string& message);

private:
    unsigned short           port_           ;

    SSL_CTX* sslContext_                     ;
    std::string certFile = "/home/dataDrill//dataDrill.crt";
    std::string keyFile  = "/home/dataDrill/dataDrill.key";

    std::atomic<bool>        serverRunning_  ;
    std::vector<std::thread> clientThreads_  ;
    std::mutex               clientMutex_    ;
    std::condition_variable  clientCondition_;

    static std::mutex              broadcastMutex_  ;
    static std::vector<int>        clientSockets_   ;
    static std::queue<std::string> messageQueue_    ;
    static std::condition_variable messageCondition_;
    static std::atomic<bool> shutdownRequested_; // Static member declaration
    
    void initializeSSLContext();
    void         acceptClients           ()                           ;
    
    void         handleClient            (int clientSocket, SSL* ssl) ;
    static void  signalHandler           (int signum)                 ;
    static void  sendMessageToAllClients (const std::string& message) ;
    void         processMessageQueue     ()                           ;
    void         logSslErrors            (const std::string& message) ;
    static std::map<int, SSL*> clientSslMap_; // Maps client sockets to their SSL objects
    static std::mutex clientSslMapMutex_; // Mutex for protecting clientSslMap_

};

#endif // CRIO_DEBUGSERVER_H
