#ifndef RLE_H
#define RLE_H

#include "compressionAlgorithm.h"

// Run-Length Encoding implementation
class RLE : public CompressionAlgorithm {
public:
    RLE() : CompressionAlgorithm("RLE") {}
    
    bool compress(const std::vector<uint8_t>& input, 
                 std::vector<uint8_t>& output) override;
    
    bool decompress(const std::vector<uint8_t>& input, 
                   std::vector<uint8_t>& output) override;

private:
    // Maximum run length for RLE
    static constexpr uint8_t MAX_RUN_LENGTH = 255;
};

#endif // RLE_H