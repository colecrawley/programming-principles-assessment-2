#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Network configuration
constexpr int DEFAULT_PORT = 8080;
constexpr int MAX_BUFFER_SIZE = 8192;
constexpr int MAX_CONNECTIONS = 10;
constexpr const char* DEFAULT_SERVER_IP = "127.0.0.1";

// File paths
const std::string COMPRESSED_DIR = "./compressed/";
const std::string DECOMPRESSED_DIR = "./decompressed/";
const std::string TEMP_DIR = "./temp/";

// Threading configuration
constexpr int MAX_WORKER_THREADS = 5;

// Logging configuration
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR_LEVEL
};

constexpr LogLevel CURRENT_LOG_LEVEL = LogLevel::INFO;

#endif // CONFIG_H
