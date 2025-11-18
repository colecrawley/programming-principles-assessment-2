#include "fileHandler.h"
#include "logger.h"

#include <fstream>
#include <vector>
#include <string>
#include <filesystem>   // C++17
namespace fs = std::filesystem;

bool FileHandler::readFile(const std::string& filepath, std::vector<uint8_t>& data) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        Logger::error("Failed to open file for reading: " + filepath);
        return false;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(size);
    if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
        Logger::error("Failed to read file: " + filepath);
        return false;
    }
    
    Logger::info("Successfully read file: " + filepath + " (" + std::to_string(size) + " bytes)");
    return true;
}

bool FileHandler::writeFile(const std::string& filepath, const std::vector<uint8_t>& data) {
    std::ofstream file(filepath, std::ios::binary);
    
    if (!file.is_open()) {
        Logger::error("Failed to open file for writing: " + filepath);
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    if (!file.good()) {
        Logger::error("Failed to write file: " + filepath);
        return false;
    }
    
    Logger::info("Successfully wrote file: " + filepath + " (" + std::to_string(data.size()) + " bytes)");
    return true;
}

bool FileHandler::fileExists(const std::string& filepath) {
    return fs::exists(filepath);
}

size_t FileHandler::getFileSize(const std::string& filepath) {
    if (fs::exists(filepath)) {
        return fs::file_size(filepath);
    }
    return 0;
}

bool FileHandler::createDirectory(const std::string& dirPath) {
    try {
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
            Logger::info("Created directory: " + dirPath);
        }
        return true;
    } catch (const std::exception& e) {
        Logger::error("Failed to create directory: " + dirPath + " (" + e.what() + ")");
        return false;
    }
}

std::string FileHandler::getFileName(const std::string& filepath) {
    return fs::path(filepath).filename().string();
}

std::string FileHandler::getFileExtension(const std::string& filepath) {
    return fs::path(filepath).extension().string();
}

std::string FileHandler::removeExtension(const std::string& filename) {
    return fs::path(filename).stem().string();
}

std::string FileHandler::generateOutputFilename(const std::string& inputFile, 
                                                const std::string& operation,
                                                const std::string& algorithm) {
    std::string baseName = getFileName(inputFile);
    std::string nameWithoutExt = removeExtension(baseName);
    std::string extension = getFileExtension(baseName);
    
    if (operation == "compress") {
        return nameWithoutExt + "_" + algorithm + ".compressed";
    } else {
        // Remove algorithm suffix if present
        size_t pos = nameWithoutExt.find_last_of('_');
        if (pos != std::string::npos) {
            nameWithoutExt = nameWithoutExt.substr(0, pos);
        }
        return nameWithoutExt + "_decompressed" + extension;
    }
}
