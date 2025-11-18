#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include <cstdint>

class FileHandler {
public:
    // Read entire file into memory
    static bool readFile(const std::string& filepath, std::vector<uint8_t>& data);
    
    // Write data to file
    static bool writeFile(const std::string& filepath, const std::vector<uint8_t>& data);
    
    // Check if file exists
    static bool fileExists(const std::string& filepath);
    
    // Get file size
    static size_t getFileSize(const std::string& filepath);
    
    // Create directory if it doesn't exist
    static bool createDirectory(const std::string& dirPath);
    
    // Extract filename from path
    static std::string getFileName(const std::string& filepath);
    
    // Get file extension
    static std::string getFileExtension(const std::string& filepath);
    
    // Remove file extension
    static std::string removeExtension(const std::string& filename);
    
    // Generate output filename for compressed/decompressed file
    static std::string generateOutputFilename(const std::string& inputFile, 
                                             const std::string& operation,
                                             const std::string& algorithm);
};

#endif // FILE_HANDLER_H