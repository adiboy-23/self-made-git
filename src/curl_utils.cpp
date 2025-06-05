#include "../include/git_utils.h"
#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <openssl/sha.h>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

size_t write_callback(void *receivedData, size_t elementSize,
                      size_t numElements, void *userData) {
    size_t totalSize = elementSize * numElements;
    std::string receivedText(static_cast<char *>(receivedData), totalSize);
    auto *masterHash = static_cast<std::string *>(userData);

    // Debug output
    std::cerr << "Received response: " << receivedText << "\n";

    // Parse the Git protocol response
    std::istringstream iss(receivedText);
    std::string line;
    while (std::getline(iss, line)) {
        // Skip service advertisement and capability lines
        if (line.find("service=git-upload-pack") != std::string::npos ||
            line.find("multi_ack") != std::string::npos) {
            continue;
        }
        
        // Look for the refs/heads/main line
        if (line.find("refs/heads/main") != std::string::npos) {
            // Extract the hash from the pkt-line format
            // Format is: <length><hash>refs/heads/main
            size_t hashStart = 4;  // Skip the 4-digit length prefix
            if (line.length() >= hashStart + 40) {
                std::string potentialHash = line.substr(hashStart, 40);
                if (std::all_of(potentialHash.begin(), potentialHash.end(), ::isxdigit)) {
                    *masterHash = potentialHash;
                    std::cerr << "Found hash: " << *masterHash << "\n";
                    return totalSize;
                }
            }
        }
    }

    return totalSize;
}

size_t pack_data_callback(void *receivedData, size_t elementSize,
                          size_t numElements, void *userData) {
  auto *accumulatedData = static_cast<std::string *>(userData);
  *accumulatedData +=
      std::string(static_cast<char *>(receivedData), elementSize * numElements);
  return elementSize * numElements;
}

std::pair<std::string, std::string> curl_request(const std::string &url) {
  CURL *handle = curl_easy_init();
  if (!handle) {
    std::cerr << "Failed to initialize cURL.\n";
    return {};
  }

  // Add debugging
  curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(handle, CURLOPT_USERAGENT, "git/1.0");
  curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);

  // Format URL
  std::string formatted_url = url;
  if (formatted_url.find("https://") == std::string::npos) {
    formatted_url = "https://" + formatted_url;
  }
  if (formatted_url.length() > 4 && 
      formatted_url.substr(formatted_url.length() - 4) == ".git") {
    formatted_url = formatted_url.substr(0, formatted_url.length() - 4);
  }

  std::string packHash;
  std::string info_refs_url = formatted_url + "/info/refs?service=git-upload-pack";
  std::cerr << "Requesting: " << info_refs_url << "\n";
  
  curl_easy_setopt(handle, CURLOPT_URL, info_refs_url.c_str());
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &packHash);
  
  CURLcode res = curl_easy_perform(handle);
  if (res != CURLE_OK) {
    std::cerr << "First request failed: " << curl_easy_strerror(res) << "\n";
    curl_easy_cleanup(handle);
    return {};
  }

  if (packHash.empty()) {
    std::cerr << "No pack hash received. Response might be empty or in unexpected format.\n";
    curl_easy_cleanup(handle);
    return {};
  }

  std::cerr << "Received pack hash: " << packHash << "\n";

  // Reset the handle for the second request
  curl_easy_reset(handle);
  curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(handle, CURLOPT_USERAGENT, "git/1.0");

  // Fetch git-upload-pack
  std::string upload_pack_url = formatted_url + "/git-upload-pack";
  std::cerr << "Requesting pack from: " << upload_pack_url << "\n";
  
  curl_easy_setopt(handle, CURLOPT_URL, upload_pack_url.c_str());
  std::string postData = "0032want " + packHash + "\n00000009done\n";
  curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postData.c_str());

  std::string packData;
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &packData);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, pack_data_callback);

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(
      headers, "Content-Type: application/x-git-upload-pack-request");
  curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

  res = curl_easy_perform(handle);
  if (res != CURLE_OK) {
    std::cerr << "Second request failed: " << curl_easy_strerror(res) << "\n";
    curl_easy_cleanup(handle);
    curl_slist_free_all(headers);
    return {};
  }

  // Clean up
  curl_easy_cleanup(handle);
  curl_slist_free_all(headers);

  if (packData.size() < 20) {
    std::cerr << "Received pack data is too small: " << packData.size() << " bytes\n";
    return {};
  }

  return {packData, packHash};
}
