#include "huffman.h"
#include "logger.h"
#include <iostream>
#include <cassert>
#include <string>

void testBasicCompression() {
    std::cout << "\n=== Test: Basic Compression ===" << std::endl;
    
    Huffman huffman;
    
    // Test data
    std::string testStr = "hello world! this is a test of huffman compression.";
    std::vector<uint8_t> input(testStr.begin(), testStr.end());
    std::vector<uint8_t> compressed, decompressed;
    
    // Compress
    bool compressResult = huffman.compress(input, compressed);
    assert(compressResult && "Compression should succeed");
    std::cout << "Original size: " << input.size() << " bytes" << std::endl;
    std::cout << "Compressed size: " << compressed.size() << " bytes" << std::endl;
    
    // Decompress
    bool decompressResult = huffman.decompress(compressed, decompressed);
    assert(decompressResult && "Decompression should succeed");
    std::cout << "Decompressed size: " << decompressed.size() << " bytes" << std::endl;
    
    // Verify
    assert(input == decompressed && "Decompressed data should match original");
    std::cout << "✓ Data integrity verified" << std::endl;
}

void testEmptyData() {
    std::cout << "\n=== Test: Empty Data ===" << std::endl;
    
    Huffman huffman;
    std::vector<uint8_t> input, compressed, decompressed;
    
    bool result = huffman.compress(input, compressed);
    assert(result && "Empty compression should succeed");
    assert(compressed.empty() && "Compressed empty data should be empty");
    
    std::cout << "✓ Empty data handled correctly" << std::endl;
}

void testSingleByte() {
    std::cout << "\n=== Test: Single Byte ===" << std::endl;
    
    Huffman huffman;
    std::vector<uint8_t> input = {65}; // 'A'
    std::vector<uint8_t> compressed, decompressed;
    
    bool compressResult = huffman.compress(input, compressed);
    assert(compressResult && "Single byte compression should succeed");
    
    bool decompressResult = huffman.decompress(compressed, decompressed);
    assert(decompressResult && "Single byte decompression should succeed");
    assert(input == decompressed && "Single byte should decompress correctly");
    
    std::cout << "✓ Single byte handled correctly" << std::endl;
}

void testRepeatedBytes() {
    std::cout << "\n=== Test: Repeated Bytes ===" << std::endl;
    
    Huffman huffman;
    std::vector<uint8_t> input(1000, 'A'); // 1000 repetitions of 'A'
    std::vector<uint8_t> compressed, decompressed;
    
    bool compressResult = huffman.compress(input, compressed);
    assert(compressResult && "Repeated bytes compression should succeed");
    std::cout << "Original size: " << input.size() << " bytes" << std::endl;
    std::cout << "Compressed size: " << compressed.size() << " bytes" << std::endl;
    
    // Should achieve good compression
    assert(compressed.size() < input.size() && "Compression should reduce size");
    
    bool decompressResult = huffman.decompress(compressed, decompressed);
    assert(decompressResult && "Repeated bytes decompression should succeed");
    assert(input == decompressed && "Data should match after decompression");
    
    std::cout << "✓ Repeated bytes handled correctly" << std::endl;
}

void testBinaryData() {
    std::cout << "\n=== Test: Binary Data ===" << std::endl;
    
    Huffman huffman;
    std::vector<uint8_t> input;
    
    // Create binary data with all byte values
    for (int i = 0; i < 256; i++) {
        input.push_back(static_cast<uint8_t>(i));
    }
    
    std::vector<uint8_t> compressed, decompressed;
    
    bool compressResult = huffman.compress(input, compressed);
    assert(compressResult && "Binary data compression should succeed");
    
    bool decompressResult = huffman.decompress(compressed, decompressed);
    assert(decompressResult && "Binary data decompression should succeed");
    assert(input == decompressed && "Binary data should match after decompression");
    
    std::cout << "✓ Binary data handled correctly" << std::endl;
}

int main() {
    Logger::init("test_huffman.log");
    
    std::cout << "========================================" << std::endl;
    std::cout << "      Huffman Algorithm Tests          " << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testBasicCompression();
        testEmptyData();
        testSingleByte();
        testRepeatedBytes();
        testBinaryData();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  All tests passed successfully! ✓    " << std::endl;
        std::cout << "========================================" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        Logger::close();
        return 1;
    }
    
    Logger::close();
    return 0;
}