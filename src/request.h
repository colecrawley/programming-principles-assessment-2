#ifndef REQUEST_H
#define REQUEST_H

#include "messageTypes.h"
#include <vector>
#include <string>
#include <winsock2.h> // SOCKET

// Encapsulates a client request
class Request {
private:
    MessageType messageType;
    AlgorithmType algorithmType;
    std::string filename;
    std::vector<uint8_t> data;

public:
    Request();
    Request(MessageType msgType, AlgorithmType algoType, 
            const std::string& fname, const std::vector<uint8_t>& fileData);
    
    // Getters
    MessageType getMessageType() const { return messageType; }
    AlgorithmType getAlgorithmType() const { return algorithmType; }
    std::string getFilename() const { return filename; }
    const std::vector<uint8_t>& getData() const { return data; }
    
    // Setters
    void setMessageType(MessageType type) { messageType = type; }
    void setAlgorithmType(AlgorithmType type) { algorithmType = type; }
    void setFilename(const std::string& fname) { filename = fname; }
    void setData(const std::vector<uint8_t>& fileData) { data = fileData; }
    
    // Serialization
    bool serialize(SOCKET sock) const;
    bool deserialize(SOCKET sock);
    
    // Display request info
    void print() const;
};

#endif // REQUEST_H
