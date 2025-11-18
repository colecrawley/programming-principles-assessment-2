#include "RLE.h"
#include "logger.h"
#include <iostream>
#include <cassert>
#include <string>

void testBasicCompression() {
    std::cout << "\n=== Test: Basic RLE Compression ===" << std::endl;
    
    RLE rle;
    
    // Test data with runs
    std::string testStr = "aaabbbcccdddeee";
    std::vector<uint8_t> input(testStr.begin(), testStr.end());
    std::vector<uint8_t> compressed, decompressed;
    
    // Compress
    bool compressResult = rle.compress(input, compressed);
    assert(compressResult && "Compression should succeed");
    std::cout << "Original size: " << input.size() << " bytes" << std::endl;
    std::cout << "Compressed size: " << compressed.size() << " bytes" << std::endl;
    
    // Decompress
    bool decompressResult = rle.decompress(compressed, decompressed);
    assert(decompressResult && "Decompression should succeed");
    std::cout << "Decompressed size: " << decompressed.size() << " bytes" << std::endl;
    
    // Verify
    assert(input == decompressed && "Decompressed data should match original");
    std::cout << "✓ Data integrity verified" << std::endl;
}

void testEmptyData() {
    std::cout << "\n=== Test: Empty Data ===" << std::endl;
    
    RLE rle;
    std::vector<uint8_t> input, compressed, decompressed;
    
    bool result = rle.compress(input, compressed);
    assert(result && "Empty compression should succeed");
    assert(compressed.empty() && "Compressed empty data should be empty");
    
    std::cout << "✓ Empty data handled correctly" << std::endl;
}

void testSingleByte() {
    std::cout << "\n=== Test: Single Byte ===" << std::endl;
    
    RLE rle;
    std::vector<uint8_t> input = {65}; // 'A'
    std::vector<uint8_t> compressed, decompressed;
    
    bool compressResult = rle.compress(input, compressed);
    assert(compressResult && "Single byte compression should succeed");
    assert(compressed.size() == 2 && "RLE of single byte should be 2 bytes");
    assert(compressed[0] == 1 && "Count should be 1");
    assert(compressed[1] == 65 && "Value should be 65");
    
    bool decompressResult = rle.decompress(compressed, decompressed);
    assert(decompressResult && "Single byte decompression should succeed");
    assert(input == decompressed && "Single byte should decompress correctly");
    
    std::cout << "✓ Single byte handled correctly" << std::endl;
}

void testLongRun() {
    std::cout << "\n=== Test: Long Run ===" << std::endl;
    
    RLE rle;
    std::vector<uint8_t> input(1000, 'A'); // 1000 repetitions of 'A'
    std::vector<uint8_t> compressed, decompressed;
    
    bool compressResult = rle.compress(input, compressed);
    assert(compressResult && "Long run compression should succeed");
    std::cout << "Original size: " << input.size() << " bytes" << std::endl;
    std::cout << "Compressed size: " << compressed.size() << " bytes" << std::endl;
    
    // Should achieve excellent compression
    assert(compressed.size() < input.size() / 2 && "Compression should significantly reduce size");
    
    bool decompressResult = rle.decompress(compressed, decompressed);
    assert(decompressResult && "Long run decompression should succeed");
    assert(input == decompressed && "Data should match after decompression");
    
    std::cout << "✓ Long run handled correctly" << std::endl;
}

void testNoRuns() {
    std::cout << "\n=== Test: No Runs (worst case) ===" << std::endl;
    
    RLE rle;
    std::vector<uint8_t> input = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<uint8_t> compressed, decompressed;
    
    bool compressResult = rle.compress(input, compressed);
    assert(compressResult && "No runs compression should succeed");
    std::cout << "Original size: " << input.size() << " bytes" << std::endl;
    std::cout << "Compressed size: " << compressed.size() << " bytes" << std::endl;
    
    // RLE will expand data when there are no runs
    assert(compressed.size() == input.size() * 2 && "RLE should double size with no runs");
    
    bool decompressResult = rle.decompress(compressed, decompressed);
    assert(decompressResult && "No runs decompression should succeed");
    assert(input == decompressed && "Data should match after decompression");
    
    std::cout << "✓ No runs (worst case) handled correctly" << std::endl;
}

void testMixedRuns() {
    std::cout << "\n=== Test: Mixed Runs ===" << std::endl;
    
    RLE rle;
    std::vector<uint8_t> input = {1, 1, 1, 2, 3, 3, 3, 3, 4, 5, 5};
    std::vector<uint8_t> compressed, decompressed;
    
    bool compressResult = rle.compress(input, compressed);
    assert(compressResult && "Mixed runs compression should succeed");
    
    bool decompressResult = rle.decompress(compressed, decompressed);
    assert(decompressResult && "Mixed runs decompression should succeed");
    assert(input == decompressed && "Data should match after decompression");
    
    std::cout << "✓ Mixed runs handled correctly" << std::endl;
}

int main() {
    Logger::init("test_rle.log");
    
    std::cout << "========================================" << std::endl;
    std::cout << "         RLE Algorithm Tests           " << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testBasicCompression();
        testEmptyData();
        testSingleByte();
        testLongRun();
        testNoRuns();
        testMixedRuns();
        
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