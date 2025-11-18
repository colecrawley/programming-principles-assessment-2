#include "logger.h"
#include <iomanip>
#include <sstream>

std::mutex Logger::logMutex;
std::ofstream Logger::logFile;
bool Logger::fileLoggingEnabled = false;

void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);
    logFile.open(filename, std::ios::app);
    if (logFile.is_open()) {
        fileLoggingEnabled = true;
        logFile << "\n========== New Session: " << getCurrentTimestamp() << " ==========\n";
    }
}

void Logger::close() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.close();
        fileLoggingEnabled = false;
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR_LEVEL: return "ERROR";
        default: return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < CURRENT_LOG_LEVEL) return;
    
    std::lock_guard<std::mutex> lock(logMutex);
    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = levelToString(level);
    std::string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    // Console output
    std::cout << logMessage << std::endl;
    
    // File output
    if (fileLoggingEnabled && logFile.is_open()) {
        logFile << logMessage << std::endl;
        logFile.flush();
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR_LEVEL, message);
}