#include "huffman.h"
#include "logger.h"
#include <bitset>
#include <cstring>

std::map<uint8_t, uint32_t> Huffman::buildFrequencyTable(const std::vector<uint8_t>& data) {
    std::map<uint8_t, uint32_t> frequencies;
    for (uint8_t byte : data) {
        frequencies[byte]++;
    }
    return frequencies;
}

std::shared_ptr<HuffmanNode> Huffman::buildHuffmanTree(const std::map<uint8_t, uint32_t>& frequencies) {
    std::priority_queue<std::shared_ptr<HuffmanNode>, 
                       std::vector<std::shared_ptr<HuffmanNode>>,
                       CompareNodes> pq;
    
    // Create leaf nodes
    for (const auto& pair : frequencies) {
        pq.push(std::make_shared<HuffmanNode>(pair.first, pair.second));
    }
    
    // Build tree
    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        
        auto parent = std::make_shared<HuffmanNode>(0, left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        
        pq.push(parent);
    }
    
    return pq.top();
}

void Huffman::generateCodes(const std::shared_ptr<HuffmanNode>& node, 
                           const std::string& code,
                           std::map<uint8_t, std::string>& codes) {
    if (!node) return;
    
    if (node->isLeaf()) {
        codes[node->data] = code.empty() ? "0" : code;
        return;
    }
    
    generateCodes(node->left, code + "0", codes);
    generateCodes(node->right, code + "1", codes);
}

void Huffman::serializeTree(const std::shared_ptr<HuffmanNode>& node, std::vector<uint8_t>& output) {
    if (!node) return;
    
    if (node->isLeaf()) {
        output.push_back(1); // Leaf marker
        output.push_back(node->data);
    } else {
        output.push_back(0); // Internal node marker
        serializeTree(node->left, output);
        serializeTree(node->right, output);
    }
}

std::shared_ptr<HuffmanNode> Huffman::deserializeTree(const std::vector<uint8_t>& input, size_t& index) {
    if (index >= input.size()) return nullptr;
    
    uint8_t marker = input[index++];
    
    if (marker == 1) { // Leaf node
        if (index >= input.size()) return nullptr;
        uint8_t data = input[index++];
        return std::make_shared<HuffmanNode>(data, 0);
    } else { // Internal node
        auto node = std::make_shared<HuffmanNode>(0, 0);
        node->left = deserializeTree(input, index);
        node->right = deserializeTree(input, index);
        return node;
    }
}

std::vector<uint8_t> Huffman::bitStringToBytes(const std::string& bits) {
    std::vector<uint8_t> bytes;
    
    for (size_t i = 0; i < bits.length(); i += 8) {
        std::string byteStr = bits.substr(i, 8);
        while (byteStr.length() < 8) {
            byteStr += '0'; // Pad with zeros
        }
        bytes.push_back(static_cast<uint8_t>(std::bitset<8>(byteStr).to_ulong()));
    }
    
    return bytes;
}

bool Huffman::compress(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    if (input.empty()) {
        Logger::warning("Huffman: Input data is empty");
        output.clear();
        return true;
    }
    
    // Build frequency table
    auto frequencies = buildFrequencyTable(input);
    
    // Build Huffman tree
    auto root = buildHuffmanTree(frequencies);
    
    // Generate codes
    std::map<uint8_t, std::string> codes;
    generateCodes(root, "", codes);
    
    // Encode data
    std::string encodedBits;
    for (uint8_t byte : input) {
        encodedBits += codes[byte];
    }
    
    // Serialize tree
    std::vector<uint8_t> serializedTree;
    serializeTree(root, serializedTree);
    
    // Build output: [tree_size][tree][original_size][padding_bits][encoded_data]
    output.clear();
    
    // Write tree size (4 bytes)
    uint32_t treeSize = serializedTree.size();
    output.insert(output.end(), 
                  reinterpret_cast<uint8_t*>(&treeSize), 
                  reinterpret_cast<uint8_t*>(&treeSize) + sizeof(treeSize));
    
    // Write tree
    output.insert(output.end(), serializedTree.begin(), serializedTree.end());
    
    // Write original data size (4 bytes)
    uint32_t originalSize = input.size();
    output.insert(output.end(),
                  reinterpret_cast<uint8_t*>(&originalSize),
                  reinterpret_cast<uint8_t*>(&originalSize) + sizeof(originalSize));
    
    // Write padding bits count (1 byte)
    uint8_t paddingBits = (8 - (encodedBits.length() % 8)) % 8;
    output.push_back(paddingBits);
    
    // Write encoded data
    auto encodedBytes = bitStringToBytes(encodedBits);
    output.insert(output.end(), encodedBytes.begin(), encodedBytes.end());
    
    Logger::info("Huffman Compression: " + std::to_string(input.size()) + 
                 " bytes -> " + std::to_string(output.size()) + " bytes");
    return true;
}

bool Huffman::decodeData(const std::shared_ptr<HuffmanNode>& root, 
                        const std::vector<uint8_t>& encodedData,
                        uint32_t originalSize,
                        std::vector<uint8_t>& output) {
    output.clear();
    output.reserve(originalSize);
    
    auto currentNode = root;
    
    for (size_t i = 0; i < encodedData.size() && output.size() < originalSize; i++) {
        std::bitset<8> bits(encodedData[i]);
        
        for (int j = 7; j >= 0 && output.size() < originalSize; j--) {
            if (bits[j]) {
                currentNode = currentNode->right;
            } else {
                currentNode = currentNode->left;
            }
            
            if (currentNode->isLeaf()) {
                output.push_back(currentNode->data);
                currentNode = root;
            }
        }
    }
    
    return output.size() == originalSize;
}

bool Huffman::decompress(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    if (input.empty()) {
        Logger::warning("Huffman: Input data is empty");
        output.clear();
        return true;
    }
    
    size_t index = 0;
    
    // Read tree size
    if (index + sizeof(uint32_t) > input.size()) {
        Logger::error("Huffman: Invalid compressed data (tree size)");
        return false;
    }
    uint32_t treeSize;
    std::memcpy(&treeSize, input.data() + index, sizeof(treeSize));
    index += sizeof(treeSize);
    
    // Read tree
    if (index + treeSize > input.size()) {
        Logger::error("Huffman: Invalid compressed data (tree)");
        return false;
    }
    auto root = deserializeTree(input, index);
    
    // Read original size
    if (index + sizeof(uint32_t) > input.size()) {
        Logger::error("Huffman: Invalid compressed data (original size)");
        return false;
    }
    uint32_t originalSize;
    std::memcpy(&originalSize, input.data() + index, sizeof(originalSize));
    index += sizeof(originalSize);
    
    // Read padding bits
    if (index >= input.size()) {
        Logger::error("Huffman: Invalid compressed data (padding)");
        return false;
    }
    uint8_t paddingBits = input[index++];
    
    // Read encoded data
    std::vector<uint8_t> encodedData(input.begin() + index, input.end());
    
    // Decode
    bool success = decodeData(root, encodedData, originalSize, output);
    
    if (success) {
        Logger::info("Huffman Decompression: " + std::to_string(input.size()) + 
                     " bytes -> " + std::to_string(output.size()) + " bytes");
    } else {
        Logger::error("Huffman: Decompression failed");
    }
    
    return success;
}