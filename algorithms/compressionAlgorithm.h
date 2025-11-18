#ifndef COMPRESSION_ALGORITHM_H
#define COMPRESSION_ALGORITHM_H

#include <vector>
#include <string>
#include <cstdint>

// Abstract base class for compression algorithms (OOP - Polymorphism)
class CompressionAlgorithm {
protected:
    std::string algorithmName;
    
public:
    CompressionAlgorithm(const std::string& name) : algorithmName(name) {}
    virtual ~CompressionAlgorithm() = default;
    
    // Pure virtual functions - must be implemented by derived classes
    virtual bool compress(const std::vector<uint8_t>& input, 
                         std::vector<uint8_t>& output) = 0;
    
    virtual bool decompress(const std::vector<uint8_t>& input, 
                           std::vector<uint8_t>& output) = 0;
    
    // Getter for algorithm name
    std::string getName() const { return algorithmName; }
    
    // Calculate compression ratio
    double calculateCompressionRatio(size_t originalSize, size_t compressedSize) const {
        if (originalSize == 0) return 0.0;
        return (1.0 - static_cast<double>(compressedSize) / originalSize) * 100.0;
    }
};

#endif // COMPRESSION_ALGORITHM_H