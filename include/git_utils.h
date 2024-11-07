#ifndef GIT_UTILS_H
#define GIT_UTILS_H

#include <string>
#include <vector>
#include <cstdio>

// Constants
#define CHUNK 16384 // 16KB

// Initialization
bool init(const std::string &dir = ".");

// Object and File Operations
bool cat_file(int argc, char *argv[]);

bool decompress_object(std::string &buf, std::string data);

int decompress_file(FILE *input, FILE *output);

std::string decompress_str(const std::string &compressed_str);

std::string compress_str(const std::string &str);

void compress_to_file(const std::string &hash, const std::string &content, const std::string &dir = ".");

// Tree Operations
bool read_tree(int argc, char *argv[]);

std::string write_tree(const std::string &dest);

void restore_tree(const std::string &hash, const std::string &dest,
                  const std::string &repo_dir);

// Blob Operations
std::string create_blob(const std::string &path);

// Utility Functions
std::string hex_to_binary(const std::string &hex);

std::string sha1(const std::string &data);

std::string digest_to_hash(const std::string &digest);

// Git Operations
bool commit_tree(int argc, char *argv[]);

bool clone(int argc, char *argv[]);

// Callback Functions
size_t write_callback(void *received_data, size_t element_size, size_t num_element, void *userdata);

size_t pack_data_callback(void *received_data, size_t element_size, size_t num_element, void *userdata);

// Network Operations
std::pair<std::string, std::string> curl_request(const std::string &url);

#endif // GIT_UTILS_H


// #ifndef GIT_UTILS_H
// #define GIT_UTILS_H

// #include <string>
// #include <vector>
// #include <cstdio>

// // Constants
// #define CHUNK 16384 // 16KB buffer size for compression operations

// /**
//  * Core Git Commands
//  */

// // Initialize a new Git repository
// bool init(const std::string &dir = ".");

// // Display the contents of a Git object
// bool cat_file(int argc, char *argv[]);

// // Create a Git blob object from a file
// std::string create_blob(const std::string &path);

// /**
//  * Tree Operations
//  */

// // List the contents of a tree object
// bool read_tree(int argc, char *argv[]);

// // Create a tree object from current directory
// std::string write_tree(const std::string &dest);

// // Restore a tree object to the filesystem
// void restore_tree(const std::string &hash, const std::string &dest,
//                  const std::string &repo_dir);

// /**
//  * Compression Utilities
//  */

// // Decompress a Git object into a buffer
// bool decompress_object(std::string &buf, std::string data);

// // Decompress a file stream
// int decompress_file(FILE *input, FILE *output);

// // Decompress a string
// std::string decompress_str(const std::string &compressed_str);

// // Compress a string
// std::string compress_str(const std::string &str);

// // Compress and write content to a Git object file
// void compress_to_file(const std::string &hash, const std::string &content, 
//                      const std::string &dir = ".");

// /**
//  * Hash Utilities
//  */

// // Convert a hexadecimal string to binary
// std::string hex_to_binary(const std::string &hex);

// // Calculate SHA1 hash of data
// std::string sha1(const std::string &data);

// // Convert a binary hash digest to hexadecimal string
// std::string digest_to_hash(const std::string &digest);

// /**
//  * Advanced Git Operations
//  */

// // Create a commit object
// bool commit_tree(int argc, char *argv[]);

// // Clone a remote repository
// bool clone(int argc, char *argv[]);

// /**
//  * Network Utilities
//  */

// // Callback for handling received data
// size_t write_callback(void *received_data, size_t element_size, 
//                      size_t num_element, void *userdata);

// // Callback for handling pack file data
// size_t pack_data_callback(void *received_data, size_t element_size, 
//                          size_t num_element, void *userdata);

// // Make HTTP request to Git server
// std::pair<std::string, std::string> curl_request(const std::string &url);

// #endif // GIT_UTILS_H