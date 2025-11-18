#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "request.h"
#include "response.h"
#include <string>
#include <vector>
#include <winsock2.h> // SOCKET

class WorkerThread {
private:
    SOCKET clientSocket; // Use SOCKET type on Windows
    
    // Process compression request
    bool processCompression(const Request& request, Response& response);
    
    // Process decompression request
    bool processDecompression(const Request& request, Response& response);
    
    // Save processed file
    bool saveProcessedFile(const std::string& filename, 
                          const std::vector<uint8_t>& data,
                          const std::string& operation);

public:
    WorkerThread(SOCKET socket);
    ~WorkerThread();
    
    // Main processing function
    void processRequest();
};

#endif // WORKERTHREAD_H