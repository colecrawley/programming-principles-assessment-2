#include "response.h"
#include "networkUtils.h"
#include "logger.h"
#include <iostream>

Response::Response()
    : status(OperationStatus::SUCCESS), filename(""), message(""), data() {}

Response::Response(OperationStatus stat, const std::string& fname,
                  const std::string& msg, const std::vector<uint8_t>& fileData)
    : status(stat), filename(fname), message(msg), data(fileData) {}

bool Response::serialize(SOCKET sock) const {
    ResponseHeader header{};
    header.status = status;
    header.dataSize = static_cast<uint32_t>(data.size());
    header.fileNameLength = static_cast<uint32_t>(filename.size());
    header.messageLength = static_cast<uint32_t>(message.size());

    if (!NetworkUtils::sendData(sock, &header, sizeof(header))) {
        Logger::error("Failed to send response header");
        return false;
    }

    if (!filename.empty() && !NetworkUtils::sendData(sock, filename.data(), filename.size())) {
        Logger::error("Failed to send filename");
        return false;
    }

    if (!message.empty() && !NetworkUtils::sendData(sock, message.data(), message.size())) {
        Logger::error("Failed to send message");
        return false;
    }

    if (!data.empty() && !NetworkUtils::sendData(sock, data.data(), data.size())) {
        Logger::error("Failed to send file data");
        return false;
    }

    Logger::info("Response sent: Status=" + operationStatusToString(status) +
                 ", File=" + filename + ", Size=" + std::to_string(data.size()));
    return true;
}

bool Response::deserialize(SOCKET sock) {
    ResponseHeader header{};
    if (!NetworkUtils::receiveData(sock, &header, sizeof(header))) {
        Logger::error("Failed to receive response header");
        return false;
    }

    status = header.status;

    if (header.fileNameLength > 0) {
        std::vector<char> buf(header.fileNameLength);
        if (!NetworkUtils::receiveData(sock, buf.data(), header.fileNameLength)) return false;
        filename.assign(buf.begin(), buf.end());
    }

    if (header.messageLength > 0) {
        std::vector<char> buf(header.messageLength);
        if (!NetworkUtils::receiveData(sock, buf.data(), header.messageLength)) return false;
        message.assign(buf.begin(), buf.end());
    }

    if (header.dataSize > 0) {
        data.resize(header.dataSize);
        if (!NetworkUtils::receiveData(sock, data.data(), header.dataSize)) return false;
    }

    Logger::info("Response received: Status=" + operationStatusToString(status) +
                 ", File=" + filename + ", Size=" + std::to_string(data.size()));
    return true;
}

void Response::print() const {
    std::cout << "=== Response Details ===\n"
              << "Status: " << operationStatusToString(status) << "\n"
              << "Filename: " << filename << "\n"
              << "Message: " << message << "\n"
              << "Data Size: " << data.size() << " bytes\n"
              << "========================\n";
}
