#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>
#include <chrono>
#include <ctime>
#include "config.h"

class Logger {
private:
    static std::mutex logMutex;
    static std::ofstream logFile;
    static bool fileLoggingEnabled;
    
    static std::string getCurrentTimestamp();
    static std::string levelToString(LogLevel level);

public:
    static void init(const std::string& filename = "app.log");
    static void close();
    
    static void log(LogLevel level, const std::string& message);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
};

#endif // LOGGER_H