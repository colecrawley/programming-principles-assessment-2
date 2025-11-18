#include "server.h"
#include "logger.h"
#include "config.h"
#include <iostream>
#include <csignal>
#include <atomic>

std::atomic<bool> shutdownRequested(false);
Server* globalServer = nullptr;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutdown signal received..." << std::endl;
        shutdownRequested = true;
        if (globalServer) {
            globalServer->stop();
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize logger
    Logger::init("server.log");
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Distributed File Compression Server  " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments
    int port = DEFAULT_PORT;
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
            if (port < 1024 || port > 65535) {
                std::cerr << "Invalid port number. Using default: " << DEFAULT_PORT << std::endl;
                port = DEFAULT_PORT;
            }
        } catch (...) {
            std::cerr << "Invalid port argument. Using default: " << DEFAULT_PORT << std::endl;
            port = DEFAULT_PORT;
        }
    }
    
    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Create and start server
    Server server(port);
    globalServer = &server;
    
    std::cout << "Starting server on port " << port << "..." << std::endl;
    std::cout << "Press Ctrl+C to stop the server." << std::endl;
    std::cout << std::endl;
    
    if (!server.start()) {
        std::cerr << "Failed to start server!" << std::endl;
        Logger::error("Server startup failed");
        Logger::close();
        return 1;
    }
    
    // Server is now running and will block in acceptConnections()
    // When stop() is called (via signal handler), it will exit
    
    std::cout << "Server stopped." << std::endl;
    Logger::info("Server shutdown complete");
    Logger::close();
    
    return 0;
}