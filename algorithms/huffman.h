#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "compressionAlgorithm.h"
#include <map>
#include <queue>
#include <memory>

// Huffman tree node
struct HuffmanNode {
    uint8_t data;
    uint32_t frequency;
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;
    
    HuffmanNode(uint8_t d, uint32_t f) 
        : data(d), frequency(f), left(nullptr), right(nullptr) {}
    
    bool isLeaf() const { return !left && !right; }
};

// Comparator for priority queue
struct CompareNodes {
    bool operator()(const std::shared_ptr<HuffmanNode>& a, 
                   const std::shared_ptr<HuffmanNode>& b) const {
        return a->frequency > b->frequency;
    }
};

// Huffman Coding implementation
class Huffman : public CompressionAlgorithm {
public:
    Huffman() : CompressionAlgorithm("Huffman") {}
    
    bool compress(const std::vector<uint8_t>& input, 
                 std::vector<uint8_t>& output) override;
    
    bool decompress(const std::vector<uint8_t>& input, 
                   std::vector<uint8_t>& output) override;

private:
    // Build frequency table
    std::map<uint8_t, uint32_t> buildFrequencyTable(const std::vector<uint8_t>& data);
    
    // Build Huffman tree
    std::shared_ptr<HuffmanNode> buildHuffmanTree(const std::map<uint8_t, uint32_t>& frequencies);
    
    // Generate codes from tree
    void generateCodes(const std::shared_ptr<HuffmanNode>& node, 
                      const std::string& code,
                      std::map<uint8_t, std::string>& codes);
    
    // Serialize tree for storage
    void serializeTree(const std::shared_ptr<HuffmanNode>& node, std::vector<uint8_t>& output);
    
    // Deserialize tree from storage
    std::shared_ptr<HuffmanNode> deserializeTree(const std::vector<uint8_t>& input, size_t& index);
    
    // Convert bit string to bytes
    std::vector<uint8_t> bitStringToBytes(const std::string& bits);
    
    // Decode using Huffman tree
    bool decodeData(const std::shared_ptr<HuffmanNode>& root, 
                   const std::vector<uint8_t>& encodedData,
                   uint32_t originalSize,
                   std::vector<uint8_t>& output);
};

#endif // HUFFMAN_H