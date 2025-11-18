#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <winsock2.h>
#include <string>
#include <vector>
#include <cstdint>

class NetworkUtils {
public:
    // Send and receive raw data
    static bool sendData(SOCKET socket, const void* data, uint64_t size);
    static bool receiveData(SOCKET socket, void* buffer, uint64_t size);

    // Send and receive strings
    static bool sendString(SOCKET socket, const std::string& str);
    static bool receiveString(SOCKET socket, std::string& str, uint32_t length);

    // Send and receive binary data
    static bool sendBinaryData(SOCKET socket, const std::vector<uint8_t>& data);
    static bool receiveBinaryData(SOCKET socket, std::vector<uint8_t>& data, uint32_t size);

    // Utility functions
    static void closeSocket(SOCKET socket);
    static bool setSocketTimeout(SOCKET socket, int seconds);
};

#endif // NETWORK_UTILS_H