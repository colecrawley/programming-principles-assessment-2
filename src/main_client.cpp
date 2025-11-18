#include "client.h"
#include "logger.h"
#include "config.h"
#include "algorithmFactory.h"
#include <iostream>
#include <string>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -s, --server <IP>       Server IP address (default: " << DEFAULT_SERVER_IP << ")" << std::endl;
    std::cout << "  -p, --port <PORT>       Server port (default: " << DEFAULT_PORT << ")" << std::endl;
    std::cout << "  -c, --compress <FILE>   Compress the specified file" << std::endl;
    std::cout << "  -d, --decompress <FILE> Decompress the specified file" << std::endl;
    std::cout << "  -a, --algorithm <ALG>   Algorithm to use (huffman|rle, default: huffman)" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << programName << " -c myfile.txt" << std::endl;
    std::cout << "  " << programName << " -d myfile.compressed -a huffman" << std::endl;
    std::cout << "  " << programName << " -s 192.168.1.100 -p 8080 -c document.pdf" << std::endl;
}

void interactiveMode(Client& client) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     Interactive Client Mode            " << std::endl;
    std::cout << "========================================" << std::endl;
    
    while (true) {
        std::cout << "\nOptions:" << std::endl;
        std::cout << "1. Compress a file" << std::endl;
        std::cout << "2. Decompress a file" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "\nEnter your choice (1-3): ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == 3) {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        
        if (choice != 1 && choice != 2) {
            std::cout << "Invalid choice. Please try again." << std::endl;
            continue;
        }
        
        std::cout << "Enter file path: ";
        std::string filepath;
        std::getline(std::cin, filepath);
        
        std::cout << "Select algorithm:" << std::endl;
        std::cout << "1. Huffman" << std::endl;
        std::cout << "2. RLE" << std::endl;
        std::cout << "Enter choice (1-2): ";
        int algoChoice;
        std::cin >> algoChoice;
        std::cin.ignore();
        
        AlgorithmType algorithm = (algoChoice == 2) ? AlgorithmType::RLE : AlgorithmType::HUFFMAN;
        
        std::cout << "\n----- Processing -----" << std::endl;
        
        if (choice == 1) {
            client.compressFile(filepath, algorithm);
        } else {
            client.decompressFile(filepath, algorithm);
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize logger
    Logger::init("client.log");
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Distributed File Compression Client  " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Default values
    std::string serverIP = DEFAULT_SERVER_IP;
    int port = DEFAULT_PORT;
    std::string filepath;
    std::string operation; // "compress" or "decompress"
    AlgorithmType algorithm = AlgorithmType::HUFFMAN;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-s" || arg == "--server") {
            if (i + 1 < argc) {
                serverIP = argv[++i];
            }
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                port = std::stoi(argv[++i]);
            }
        } else if (arg == "-c" || arg == "--compress") {
            if (i + 1 < argc) {
                operation = "compress";
                filepath = argv[++i];
            }
        } else if (arg == "-d" || arg == "--decompress") {
            if (i + 1 < argc) {
                operation = "decompress";
                filepath = argv[++i];
            }
        } else if (arg == "-a" || arg == "--algorithm") {
            if (i + 1 < argc) {
                algorithm = AlgorithmFactory::getAlgorithmType(argv[++i]);
            }
        }
    }
    
    // Create client
    Client client(serverIP, port);
    
    std::cout << "Server: " << serverIP << ":" << port << std::endl;
    std::cout << std::endl;
    
    // If no operation specified, enter interactive mode
    if (operation.empty()) {
        interactiveMode(client);
    } else {
        // Execute command line operation
        bool success = false;
        
        if (operation == "compress") {
            success = client.compressFile(filepath, algorithm);
        } else if (operation == "decompress") {
            success = client.decompressFile(filepath, algorithm);
        }
        
        Logger::close();
        return success ? 0 : 1;
    }
    
    Logger::close();
    return 0;
}