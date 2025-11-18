#ifndef CLIENT_H
#define CLIENT_H

#include "request.h"
#include "response.h"
#include <string>

class Client {
private:
    std::string serverIP;
    int serverPort;
    int clientSocket;
    
    // Connect to server
    bool connectToServer();
    
    // Disconnect from server
    void disconnect();

public:
    Client(const std::string& ip, int port);
    ~Client();
    
    // Send compression request
    bool compressFile(const std::string& filepath, AlgorithmType algorithm);
    
    // Send decompression request
    bool decompressFile(const std::string& filepath, AlgorithmType algorithm);
    
    // Generic request sending
    bool sendRequest(const Request& request, Response& response);
};

#endif // CLIENT_H