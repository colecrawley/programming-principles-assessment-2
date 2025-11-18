#include "networkUtils.h"
#include "logger.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

bool NetworkUtils::sendData(SOCKET socket, const void* data, uint64_t size) {
    uint64_t totalSent = 0;
    const uint8_t* ptr = static_cast<const uint8_t*>(data);

    while (totalSent < size) {
        int sent = send(socket,
                        reinterpret_cast<const char*>(ptr + totalSent),
                        static_cast<int>(size - totalSent),
                        0);

        if (sent == SOCKET_ERROR) {
            int err = WSAGetLastError();
            Logger::error("Failed to send data: " + std::to_string(err));
            return false;
        }

        totalSent += sent;
    }

    return true;
}

bool NetworkUtils::receiveData(SOCKET socket, void* buffer, uint64_t size) {
    uint64_t totalReceived = 0;
    uint8_t* ptr = static_cast<uint8_t*>(buffer);

    while (totalReceived < size) {
        int received = recv(socket,
                            reinterpret_cast<char*>(ptr + totalReceived),
                            static_cast<int>(size - totalReceived),
                            0);

        if (received == 0) {
            Logger::warning("Connection closed by peer");
            return false;
        }

        if (received == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAETIMEDOUT) {
                Logger::warning("Receive timeout reached");
            } else {
                Logger::error("Failed to receive data: " + std::to_string(err));
            }
            return false;
        }

        totalReceived += received;
    }

    return true;
}

bool NetworkUtils::sendString(SOCKET socket, const std::string& str) {
    uint32_t length = htonl(static_cast<uint32_t>(str.size()));

    if (!sendData(socket, &length, sizeof(length)))
        return false;

    if (!str.empty())
        return sendData(socket, str.data(), str.size());

    return true;
}

bool NetworkUtils::receiveString(SOCKET socket, std::string& str, uint32_t length) {
    if (length == 0) {
        str.clear();
        return true;
    }

    str.resize(length);
    return receiveData(socket, str.data(), length);
}

bool NetworkUtils::sendBinaryData(SOCKET socket, const std::vector<uint8_t>& data) {
    uint32_t size = htonl(static_cast<uint32_t>(data.size()));

    if (!sendData(socket, &size, sizeof(size)))
        return false;

    if (!data.empty())
        return sendData(socket, data.data(), data.size());

    return true;
}

bool NetworkUtils::receiveBinaryData(SOCKET socket, std::vector<uint8_t>& data, uint32_t size) {
    if (size == 0) {
        data.clear();
        return true;
    }

    data.resize(size);
    return receiveData(socket, data.data(), size);
}

void NetworkUtils::closeSocket(SOCKET socket) {
    if (socket != INVALID_SOCKET) {
        closesocket(socket);
    }
}

bool NetworkUtils::setSocketTimeout(SOCKET socket, int seconds) {
    DWORD timeout = seconds * 1000;

    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&timeout), sizeof(timeout)) < 0) {
        Logger::error("Failed to set receive timeout: " + std::to_string(WSAGetLastError()));
        return false;
    }

    if (setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO,
                   reinterpret_cast<const char*>(&timeout), sizeof(timeout)) < 0) {
        Logger::error("Failed to set send timeout: " + std::to_string(WSAGetLastError()));
        return false;
    }

    return true;
}
