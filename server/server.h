#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <winsock2.h>

class Server {
private:
    SOCKET serverSocket;
    int port;
    std::atomic<bool> running;
    std::vector<std::thread> workerThreads;

    // Thread pool for handling client requests
    std::queue<SOCKET> clientQueue; // Use SOCKET
    std::mutex queueMutex;
    std::condition_variable queueCondition;

    // Initialize server socket
    bool initializeSocket();

    // Accept client connections
    void acceptConnections();

    // Worker thread function
    void workerThreadFunction();

public:
    Server(int portNum = 8080);
    ~Server();

    // Start the server
    bool start();

    // Stop the server
    void stop();

    // Check if server is running
    bool isRunning() const { return running; }
};

#endif // SERVER_H
