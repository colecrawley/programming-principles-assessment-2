#include "client.h"
#include "fileHandler.h"
#include "networkUtils.h"
#include "logger.h"
#include "config.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

Client::Client(const std::string& ip, int port) 
    : serverIP(ip), serverPort(port), clientSocket(INVALID_SOCKET) 
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        Logger::error("WSAStartup failed");
    }

    Logger::info("Client initialized for server " + ip + ":" + std::to_string(port));
}

Client::~Client() {
    disconnect();
    WSACleanup(); // Cleanup Winsock
}

bool Client::connectToServer() {
    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        Logger::error("Failed to create client socket");
        return false;
    }
    
    // Setup server address
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        Logger::error("Invalid server IP address");
        NetworkUtils::closeSocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }
    
    // Connect to server
    if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        Logger::error("Failed to connect to server: " + std::to_string(err));
        NetworkUtils::closeSocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }
    
    Logger::info("Connected to server " + serverIP + ":" + std::to_string(serverPort));
    return true;
}

void Client::disconnect() {
    if (clientSocket != INVALID_SOCKET) {
        NetworkUtils::closeSocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        Logger::info("Disconnected from server");
    }
}

bool Client::compressFile(const std::string& filepath, AlgorithmType algorithm) {
    std::cout << "Reading file: " << filepath << std::endl;

    std::vector<uint8_t> fileData;
    if (!FileHandler::readFile(filepath, fileData)) {
        std::cerr << "Failed to read file: " << filepath << std::endl;
        return false;
    }

    std::cout << "File size: " << fileData.size() << " bytes" << std::endl;
    std::cout << "Algorithm: " << algorithmTypeToString(algorithm) << std::endl;
    std::cout << "Connecting to server..." << std::endl;

    std::string filename = FileHandler::getFileName(filepath);
    Request request(MessageType::COMPRESS_REQUEST, algorithm, filename, fileData);

    Response response;
    if (!sendRequest(request, response)) {
        std::cerr << "Failed to send compression request" << std::endl;
        return false;
    }

    if (response.getStatus() == OperationStatus::SUCCESS) {
        std::cout << "\nCompression successful!" << std::endl;
        std::cout << "Message: " << response.getMessage() << std::endl;
        std::cout << "Output file: " << response.getFilename() << std::endl;
        std::cout << "Compressed size: " << response.getData().size() << " bytes" << std::endl;

        std::string outputPath = "./client_output/" + response.getFilename();
        FileHandler::createDirectory("./client_output");
        if (FileHandler::writeFile(outputPath, response.getData())) {
            std::cout << "Compressed file saved to: " << outputPath << std::endl;
        }
        return true;
    } else {
        std::cerr << "\nCompression failed!" << std::endl;
        std::cerr << "Error: " << response.getMessage() << std::endl;
        return false;
    }
}

bool Client::decompressFile(const std::string& filepath, AlgorithmType algorithm) {
    std::cout << "Reading file: " << filepath << std::endl;

    std::vector<uint8_t> fileData;
    if (!FileHandler::readFile(filepath, fileData)) {
        std::cerr << "Failed to read file: " << filepath << std::endl;
        return false;
    }

    std::cout << "File size: " << fileData.size() << " bytes" << std::endl;
    std::cout << "Algorithm: " << algorithmTypeToString(algorithm) << std::endl;
    std::cout << "Connecting to server..." << std::endl;

    std::string filename = FileHandler::getFileName(filepath);
    Request request(MessageType::DECOMPRESS_REQUEST, algorithm, filename, fileData);

    Response response;
    if (!sendRequest(request, response)) {
        std::cerr << "Failed to send decompression request" << std::endl;
        return false;
    }

    if (response.getStatus() == OperationStatus::SUCCESS) {
        std::cout << "\nDecompression successful!" << std::endl;
        std::cout << "Message: " << response.getMessage() << std::endl;
        std::cout << "Output file: " << response.getFilename() << std::endl;
        std::cout << "Decompressed size: " << response.getData().size() << " bytes" << std::endl;

        std::string outputPath = "./client_output/" + response.getFilename();
        FileHandler::createDirectory("./client_output");
        if (FileHandler::writeFile(outputPath, response.getData())) {
            std::cout << "Decompressed file saved to: " << outputPath << std::endl;
        }
        return true;
    } else {
        std::cerr << "\nDecompression failed!" << std::endl;
        std::cerr << "Error: " << response.getMessage() << std::endl;
        return false;
    }
}

bool Client::sendRequest(const Request& request, Response& response) {
    if (!connectToServer()) {
        return false;
    }

    std::cout << "Sending request to server..." << std::endl;

    if (!request.serialize(clientSocket)) {
        disconnect();
        return false;
    }

    std::cout << "Waiting for response..." << std::endl;

    if (!response.deserialize(clientSocket)) {
        disconnect();
        return false;
    }

    disconnect();
    return true;
}
