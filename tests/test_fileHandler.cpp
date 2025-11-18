#include "fileHandler.h"
#include "logger.h"
#include <iostream>
#include <cassert>
#include <filesystem>
namespace fs = std::filesystem;

void testReadWrite() {
    std::cout << "\n=== Test: Read/Write File ===" << std::endl;
    
    std::string testFile = "./test_data.bin";
    std::vector<uint8_t> originalData = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Write file
    bool writeResult = FileHandler::writeFile(testFile, originalData);
    assert(writeResult && "Write should succeed");
    std::cout << "✓ File written successfully" << std::endl;
    
    // Check if file exists
    bool exists = FileHandler::fileExists(testFile);
    assert(exists && "File should exist");
    std::cout << "✓ File exists" << std::endl;
    
    // Read file
    std::vector<uint8_t> readData;
    bool readResult = FileHandler::readFile(testFile, readData);
    assert(readResult && "Read should succeed");
    std::cout << "✓ File read successfully" << std::endl;
    
    // Verify data
    assert(originalData == readData && "Data should match");
    std::cout << "✓ Data integrity verified" << std::endl;
    
    // Clean up
    remove(testFile.c_str());
}

void testGetFileName() {
    std::cout << "\n=== Test: Get File Name ===" << std::endl;
    
    assert(FileHandler::getFileName("/path/to/file.txt") == "file.txt");
    assert(FileHandler::getFileName("file.txt") == "file.txt");
    assert(FileHandler::getFileName("/path/to/") == "");
    
    std::cout << "✓ File name extraction works correctly" << std::endl;
}

void testGetFileExtension() {
    std::cout << "\n=== Test: Get File Extension ===" << std::endl;
    
    assert(FileHandler::getFileExtension("file.txt") == ".txt");
    assert(FileHandler::getFileExtension("archive.tar.gz") == ".gz");
    assert(FileHandler::getFileExtension("noextension") == "");
    assert(FileHandler::getFileExtension(".hidden") == "");
    
    std::cout << "✓ File extension extraction works correctly" << std::endl;
}

void testRemoveExtension() {
    std::cout << "\n=== Test: Remove Extension ===" << std::endl;
    
    assert(FileHandler::removeExtension("file.txt") == "file");
    assert(FileHandler::removeExtension("archive.tar.gz") == "archive.tar");
    assert(FileHandler::removeExtension("noextension") == "noextension");
    
    std::cout << "✓ Extension removal works correctly" << std::endl;
}

void testGenerateOutputFilename() {
    std::cout << "\n=== Test: Generate Output Filename ===" << std::endl;
    
    std::string compressedName = FileHandler::generateOutputFilename(
        "document.txt", "compress", "Huffman");
    assert(compressedName == "document_Huffman.compressed");
    std::cout << "Compressed: " << compressedName << std::endl;
    
    std::string decompressedName = FileHandler::generateOutputFilename(
        "document_Huffman.compressed", "decompress", "Huffman");
    assert(decompressedName == "document_decompressed.compressed");
    std::cout << "Decompressed: " << decompressedName << std::endl;
    
    std::cout << "✓ Output filename generation works correctly" << std::endl;
}

void testCreateDirectory() {
    std::cout << "\n=== Test: Create Directory ===" << std::endl;
    
    std::string testDir = "./test_directory";
    
    bool created = FileHandler::createDirectory(testDir);
    assert(created && "Directory creation should succeed");
    std::cout << "✓ Directory created successfully" << std::endl;
    
    // Try creating again (should succeed as it already exists)
    bool createdAgain = FileHandler::createDirectory(testDir);
    assert(createdAgain && "Creating existing directory should succeed");
    std::cout << "✓ Existing directory handled correctly" << std::endl;
    
    // Clean up
    std::filesystem::remove(testDir);
}

int main() {
    Logger::init("test_fileHandler.log");
    
    std::cout << "========================================" << std::endl;
    std::cout << "       File Handler Tests              " << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testReadWrite();
        testGetFileName();
        testGetFileExtension();
        testRemoveExtension();
        testGenerateOutputFilename();
        testCreateDirectory();
        
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