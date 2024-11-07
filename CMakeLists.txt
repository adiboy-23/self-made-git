cmake_minimum_required(VERSION 3.13)

project(git-implementation)

set(CMAKE_CXX_STANDARD 23)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Include directories
include_directories(include)

# Add source files
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)

# Find OpenSSL with Homebrew path
set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Find and link Zlib
find_package(ZLIB REQUIRED)

# Find and link libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# Add executable
add_executable(your_program ${SOURCE_FILES})
target_link_libraries(your_program 
    ${OPENSSL_LIBRARIES} 
    ZLIB::ZLIB 
    ${CURL_LIBRARIES}
)