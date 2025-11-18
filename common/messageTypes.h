#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#include <cstdint>
#include <string>

// Message types for client-server communication
enum class MessageType : uint8_t {
    COMPRESS_REQUEST = 1,
    DECOMPRESS_REQUEST = 2,
    RESPONSE = 3,
    MSG_ERROR  = 4,  // Changed from ERROR to avoid Windows conflict
    ACK = 5
};

// Algorithm types
enum class AlgorithmType : uint8_t {
    HUFFMAN = 1,
    RLE = 2
};

// Operation status
enum class OperationStatus : uint8_t {
    SUCCESS = 0,
    FAILURE = 1,
    IN_PROGRESS = 2
};

// Message header structure
struct MessageHeader {
    MessageType type;
    AlgorithmType algorithm;
    uint32_t dataSize;
    uint32_t fileNameLength;

    MessageHeader() 
        : type(MessageType::COMPRESS_REQUEST),
          algorithm(AlgorithmType::HUFFMAN),
          dataSize(0),
          fileNameLength(0) {}
};

// Response header structure
struct ResponseHeader {
    OperationStatus status;
    uint32_t dataSize;
    uint32_t fileNameLength;
    uint32_t messageLength;

    ResponseHeader()
        : status(OperationStatus::SUCCESS),
          dataSize(0),
          fileNameLength(0),
          messageLength(0) {}
};

// Helper functions to convert enums to strings
inline std::string messageTypeToString(MessageType type) {
    switch (type) {
        case MessageType::COMPRESS_REQUEST: return "COMPRESS_REQUEST";
        case MessageType::DECOMPRESS_REQUEST: return "DECOMPRESS_REQUEST";
        case MessageType::RESPONSE: return "RESPONSE";
        case MessageType::MSG_ERROR: return "MSG_ERROR";  // Updated to match the enum change
        case MessageType::ACK: return "ACK";
        default: return "UNKNOWN"; // fallback - added default case
    }
}

inline std::string algorithmTypeToString(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::HUFFMAN: return "HUFFMAN";
        case AlgorithmType::RLE: return "RLE";
        default: return "UNKNOWN"; // fallback - added default case
    }
}

inline std::string operationStatusToString(OperationStatus status) {
    switch (status) {
        case OperationStatus::SUCCESS: return "SUCCESS";
        case OperationStatus::FAILURE: return "FAILURE";
        case OperationStatus::IN_PROGRESS: return "IN_PROGRESS";
        default: return "UNKNOWN"; // fallback - added default case
    }
}

#endif // MESSAGE_TYPES_H