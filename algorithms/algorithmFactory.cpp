#include "algorithmFactory.h"
#include "huffman.h"
#include "RLE.h"
#include "logger.h"
#include <algorithm>

std::unique_ptr<CompressionAlgorithm> AlgorithmFactory::createAlgorithm(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::HUFFMAN:
            Logger::info("Creating Huffman algorithm instance");
            return std::make_unique<Huffman>();
            
        case AlgorithmType::RLE:
            Logger::info("Creating RLE algorithm instance");
            return std::make_unique<RLE>();
            
        default:
            Logger::error("Unsupported algorithm type");
            return nullptr;
    }
}

AlgorithmType AlgorithmFactory::getAlgorithmType(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    if (lowerName == "huffman") {
        return AlgorithmType::HUFFMAN;
    } else if (lowerName == "rle") {
        return AlgorithmType::RLE;
    }
    
    Logger::warning("Unknown algorithm name: " + name + ", defaulting to Huffman");
    return AlgorithmType::HUFFMAN;
}

bool AlgorithmFactory::isSupported(AlgorithmType type) {
    return type == AlgorithmType::HUFFMAN || type == AlgorithmType::RLE;
}