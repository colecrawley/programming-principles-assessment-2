#ifndef ALGORITHM_FACTORY_H
#define ALGORITHM_FACTORY_H

#include "compressionAlgorithm.h"
#include "messageTypes.h"
#include <memory>

// Factory pattern for creating compression algorithms
class AlgorithmFactory {
public:
    // Create algorithm based on type
    static std::unique_ptr<CompressionAlgorithm> createAlgorithm(AlgorithmType type);
    
    // Get algorithm type from string
    static AlgorithmType getAlgorithmType(const std::string& name);
    
    // Check if algorithm is supported
    static bool isSupported(AlgorithmType type);
};

#endif // ALGORITHM_FACTORY_H