#ifndef RESPONSE_H
#define RESPONSE_H

#include "messageTypes.h"
#include <vector>
#include <string>
#include <winsock2.h> // SOCKET

class Response {
private:
    OperationStatus status;
    std::string filename;
    std::string message;
    std::vector<uint8_t> data;

public:
    Response();
    Response(OperationStatus stat, const std::string& fname,
            const std::string& msg, const std::vector<uint8_t>& fileData);

    // Getters
    OperationStatus getStatus() const { return status; }
    std::string getFilename() const { return filename; }
    std::string getMessage() const { return message; }
    const std::vector<uint8_t>& getData() const { return data; }

    // Setters
    void setStatus(OperationStatus stat) { status = stat; }
    void setFilename(const std::string& fname) { filename = fname; }
    void setMessage(const std::string& msg) { message = msg; }
    void setData(const std::vector<uint8_t>& fileData) { data = fileData; }

    // Serialization
    bool serialize(SOCKET sock) const;
    bool deserialize(SOCKET sock);

    // Display response info
    void print() const;
};

#endif // RESPONSE_H
