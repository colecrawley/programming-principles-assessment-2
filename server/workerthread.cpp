#include "workerthread.h"
#include "algorithmFactory.h"
#include "fileHandler.h"
#include "networkUtils.h"
#include "logger.h"
#include "config.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

WorkerThread::WorkerThread(SOCKET socket) : clientSocket(socket) {}

WorkerThread::~WorkerThread() {
    if (clientSocket != INVALID_SOCKET) {
        NetworkUtils::closeSocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
}

void WorkerThread::processRequest() {
    Logger::info("Processing request from client socket: " + std::to_string(clientSocket));
    
    Request request;
    if (!request.deserialize(clientSocket)) {
        Logger::error("Failed to receive request");
        Response errorResponse(OperationStatus::FAILURE, "", 
                               "Failed to receive request", {});
        errorResponse.serialize(clientSocket);
        return;
    }
    
    request.print();
    
    Response response;
    bool success = false;
    
    switch (request.getMessageType()) {
        case MessageType::COMPRESS_REQUEST:
            success = processCompression(request, response);
            break;
            
        case MessageType::DECOMPRESS_REQUEST:
            success = processDecompression(request, response);
            break;
            
        default:
            Logger::error("Unknown message type");
            response.setStatus(OperationStatus::FAILURE);
            response.setMessage("Unknown message type");
            break;
    }
    
    if (!response.serialize(clientSocket)) {
        Logger::error("Failed to send response");
    }
    
    response.print();
}

bool WorkerThread::processCompression(const Request& request, Response& response) {
    Logger::info("Processing compression request");
    
    auto algorithm = AlgorithmFactory::createAlgorithm(request.getAlgorithmType());
    if (!algorithm) {
        response.setStatus(OperationStatus::FAILURE);
        response.setMessage("Failed to create compression algorithm");
        return false;
    }
    
    std::vector<uint8_t> compressedData;
    if (!algorithm->compress(request.getData(), compressedData)) {
        response.setStatus(OperationStatus::FAILURE);
        response.setMessage("Compression failed");
        return false;
    }
    
    std::string outputFilename = FileHandler::generateOutputFilename(
        request.getFilename(), "compress", algorithm->getName());
    
    if (!saveProcessedFile(outputFilename, compressedData, "compress")) {
        response.setStatus(OperationStatus::FAILURE);
        response.setMessage("Failed to save compressed file");
        return false;
    }
    
    double ratio = algorithm->calculateCompressionRatio(
        request.getData().size(), compressedData.size());
    
    response.setStatus(OperationStatus::SUCCESS);
    response.setFilename(outputFilename);
    response.setData(compressedData);
    response.setMessage("Compression successful. Ratio: " + 
                        std::to_string(ratio) + "%");
    
    Logger::info("Compression completed: " + outputFilename);
    return true;
}

bool WorkerThread::processDecompression(const Request& request, Response& response) {
    Logger::info("Processing decompression request");
    
    auto algorithm = AlgorithmFactory::createAlgorithm(request.getAlgorithmType());
    if (!algorithm) {
        response.setStatus(OperationStatus::FAILURE);
        response.setMessage("Failed to create decompression algorithm");
        return false;
    }
    
    std::vector<uint8_t> decompressedData;
    if (!algorithm->decompress(request.getData(), decompressedData)) {
        response.setStatus(OperationStatus::FAILURE);
        response.setMessage("Decompression failed");
        return false;
    }
    
    std::string outputFilename = FileHandler::generateOutputFilename(
        request.getFilename(), "decompress", algorithm->getName());
    
    if (!saveProcessedFile(outputFilename, decompressedData, "decompress")) {
        response.setStatus(OperationStatus::FAILURE);
        response.setMessage("Failed to save decompressed file");
        return false;
    }
    
    response.setStatus(OperationStatus::SUCCESS);
    response.setFilename(outputFilename);
    response.setData(decompressedData);
    response.setMessage("Decompression successful. Size: " + 
                        std::to_string(decompressedData.size()) + " bytes");
    
    Logger::info("Decompression completed: " + outputFilename);
    return true;
}

bool WorkerThread::saveProcessedFile(const std::string& filename,
                                     const std::vector<uint8_t>& data,
                                     const std::string& operation) {
    std::string outputDir = (operation == "compress") ? COMPRESSED_DIR : DECOMPRESSED_DIR;
    FileHandler::createDirectory(outputDir);
    
    std::string filepath = outputDir + filename;
    return FileHandler::writeFile(filepath, data);
}
