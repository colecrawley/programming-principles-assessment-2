cmake_minimum_required(VERSION 3.10)
project(DistributedFileCompression)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Threads package for pthread support (not needed on Windows, but fine)
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/algorithms
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/file
    ${CMAKE_SOURCE_DIR}/server
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/utils
)

# Common source files shared by server, client, and tests
set(COMMON_SOURCES
    common/networkUtils.cpp
    file/fileHandler.cpp
    algorithms/RLE.cpp
    algorithms/huffman.cpp
    algorithms/algorithmFactory.cpp
    utils/logger.cpp
)

# Request/Response sources
set(MESSAGE_SOURCES
    src/request.cpp
    src/response.cpp
)

# ---------------------------
# Server executable
# ---------------------------
add_executable(server
    ${COMMON_SOURCES}
    ${MESSAGE_SOURCES}
    server/main_server.cpp
    server/server.cpp
    server/workerthread.cpp
)

# ---------------------------
# Client executable
# ---------------------------
add_executable(client
    ${COMMON_SOURCES}
    ${MESSAGE_SOURCES}
    src/main_client.cpp
    src/client.cpp
)

# ---------------------------
# Test executables
# ---------------------------
add_executable(test_huffman
    tests/test_huffman.cpp
    ${COMMON_SOURCES}
    ${MESSAGE_SOURCES}
)

add_executable(test_rle
    tests/test_rle.cpp
    ${COMMON_SOURCES}
    ${MESSAGE_SOURCES}
)

add_executable(test_fileHandler
    tests/test_fileHandler.cpp
    ${COMMON_SOURCES}
    ${MESSAGE_SOURCES}
)

# ---------------------------
# Link libraries
# ---------------------------
if(WIN32)
    set(WINDOWS_LIBS ws2_32 Threads::Threads)
else()
    set(WINDOWS_LIBS Threads::Threads)
endif()

target_link_libraries(server ${WINDOWS_LIBS})
target_link_libraries(client ${WINDOWS_LIBS})
target_link_libraries(test_huffman ${WINDOWS_LIBS})
target_link_libraries(test_rle ${WINDOWS_LIBS})
target_link_libraries(test_fileHandler ${WINDOWS_LIBS})
