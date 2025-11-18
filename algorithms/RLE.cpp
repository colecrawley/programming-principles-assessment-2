#include "RLE.h"
#include "logger.h"

bool RLE::compress(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    if (input.empty()) {
        Logger::warning("RLE: Input data is empty");
        output.clear();
        return true;
    }
    
    output.clear();
    output.reserve(input.size() * 2); // Reserve space to avoid reallocations
    
    size_t i = 0;
    while (i < input.size()) {
        uint8_t currentByte = input[i];
        uint8_t count = 1;
        
        // Count consecutive identical bytes
        while (i + count < input.size() && 
               input[i + count] == currentByte && 
               count < MAX_RUN_LENGTH) {
            count++;
        }
        
        // Write count and byte value
        output.push_back(count);
        output.push_back(currentByte);
        
        i += count;
    }
    
    Logger::info("RLE Compression: " + std::to_string(input.size()) + 
                 " bytes -> " + std::to_string(output.size()) + " bytes");
    return true;
}

bool RLE::decompress(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    if (input.empty()) {
        Logger::warning("RLE: Input data is empty");
        output.clear();
        return true;
    }
    
    if (input.size() % 2 != 0) {
        Logger::error("RLE: Invalid compressed data (odd number of bytes)");
        return false;
    }
    
    output.clear();
    
    for (size_t i = 0; i < input.size(); i += 2) {
        uint8_t count = input[i];
        uint8_t value = input[i + 1];
        
        // Append 'count' copies of 'value'
        for (uint8_t j = 0; j < count; j++) {
            output.push_back(value);
        }
    }
    
    Logger::info("RLE Decompression: " + std::to_string(input.size()) + 
                 " bytes -> " + std::to_string(output.size()) + " bytes");
    return true;
}