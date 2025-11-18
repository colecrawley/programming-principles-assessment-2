#include "server.h"
#include "workerthread.h"
#include "networkUtils.h"
#include "logger.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

Server::Server(int portNum)
    : port(portNum), serverSocket(INVALID_SOCKET), running(false)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::error("WSAStartup failed");
    }
}

Server::~Server() {
    stop();
    WSACleanup();
}

bool Server::initializeSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        Logger::error("Failed to create server socket: " + std::to_string(WSAGetLastError()));
        return false;
    }

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        Logger::error("Failed to bind socket: " + std::to_string(WSAGetLastError()));
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
        return false;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        Logger::error("Failed to listen on socket: " + std::to_string(WSAGetLastError()));
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
        return false;
    }

    Logger::info("Server listening on port " + std::to_string(port));
    return true;
}

bool Server::start() {
    if (!initializeSocket()) return false;

    running = true;

    // Start worker threads
    const size_t threadCount = std::thread::hardware_concurrency();
    for (size_t i = 0; i < threadCount; ++i) {
        workerThreads.emplace_back(&Server::workerThreadFunction, this);
    }

    // Accept connections in main thread
    acceptConnections();

    return true;
}

void Server::stop() {
    running = false;

    queueCondition.notify_all();

    for (auto& t : workerThreads) {
        if (t.joinable()) t.join();
    }
    workerThreads.clear();

    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
        Logger::info("Server stopped");
    }
}

void Server::acceptConnections() {
    while (running) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            int err = WSAGetLastError();
            if (running) Logger::error("Failed to accept client: " + std::to_string(err));
            continue;
        }

        Logger::info("Client connected");

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            clientQueue.push(clientSocket);
        }
        queueCondition.notify_one();
    }
}

void Server::workerThreadFunction() {
    while (running) {
        SOCKET clientSocket = INVALID_SOCKET;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !clientQueue.empty() || !running; });

            if (!running && clientQueue.empty()) return;

            clientSocket = clientQueue.front();
            clientQueue.pop();
        }

        if (clientSocket != INVALID_SOCKET) {
            WorkerThread worker(clientSocket);
            worker.processRequest(); // Handle the client completely inside WorkerThread
        }
    }
}