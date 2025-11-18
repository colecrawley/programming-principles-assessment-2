#include "request.h"
#include "networkUtils.h"
#include "logger.h"
#include <iostream>

Request::Request() 
    : messageType(MessageType::COMPRESS_REQUEST),
      algorithmType(AlgorithmType::HUFFMAN),
      filename(""),
      data() {}

Request::Request(MessageType msgType, AlgorithmType algoType, 
                const std::string& fname, const std::vector<uint8_t>& fileData)
    : messageType(msgType),
      algorithmType(algoType),
      filename(fname),
      data(fileData) {}

bool Request::serialize(SOCKET sock) const {
    MessageHeader header{};
    header.type = messageType;
    header.algorithm = algorithmType;
    header.dataSize = static_cast<uint32_t>(data.size());
    header.fileNameLength = static_cast<uint32_t>(filename.size());

    if (!NetworkUtils::sendData(sock, &header, sizeof(header))) {
        Logger::error("Failed to send request header");
        return false;
    }

    if (!filename.empty()) {
        if (!NetworkUtils::sendData(sock, filename.data(), filename.size())) {
            Logger::error("Failed to send filename");
            return false;
        }
    }

    if (!data.empty()) {
        if (!NetworkUtils::sendData(sock, data.data(), data.size())) {
            Logger::error("Failed to send file data");
            return false;
        }
    }

    Logger::info("Request sent: " + messageTypeToString(messageType) +
                 ", Algorithm: " + algorithmTypeToString(algorithmType) +
                 ", File: " + filename + ", Size: " + std::to_string(data.size()));
    return true;
}

bool Request::deserialize(SOCKET sock) {
    MessageHeader header{};
    if (!NetworkUtils::receiveData(sock, &header, sizeof(header))) {
        Logger::error("Failed to receive request header");
        return false;
    }

    messageType = header.type;
    algorithmType = header.algorithm;

    if (header.fileNameLength > 0) {
        std::vector<char> buf(header.fileNameLength);
        if (!NetworkUtils::receiveData(sock, buf.data(), header.fileNameLength)) {
            Logger::error("Failed to receive filename");
            return false;
        }
        filename.assign(buf.begin(), buf.end());
    }

    if (header.dataSize > 0) {
        data.resize(header.dataSize);
        if (!NetworkUtils::receiveData(sock, data.data(), header.dataSize)) {
            Logger::error("Failed to receive file data");
            return false;
        }
    }

    Logger::info("Request received: " + messageTypeToString(messageType) +
                 ", Algorithm: " + algorithmTypeToString(algorithmType) +
                 ", File: " + filename + ", Size: " + std::to_string(data.size()));
    return true;
}

void Request::print() const {
    std::cout << "=== Request Details ===\n"
              << "Message Type: " << messageTypeToString(messageType) << "\n"
              << "Algorithm: " << algorithmTypeToString(algorithmType) << "\n"
              << "Filename: " << filename << "\n"
              << "Data Size: " << data.size() << " bytes\n"
              << "======================\n";
}
