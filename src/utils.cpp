// utils.cpp
#include "utils.h"
#include <iostream>

void printProgressBar(size_t completed, size_t total, int barWidth) {
    float progress = (completed * 100.0f) / total;
    int pos = (progress / 100) * barWidth;

    std::lock_guard<std::mutex> lock(coutMutex);  // Lock cout before writing

    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            std::cout << "#";  // Filled part
        else
            std::cout << " ";  // Empty part
    }
    std::cout << "] " << (int)progress << "%" << std::flush;
}

void printUsage() {
    std::cout << "Usage: \n"
              << "  -d                           Discover peers\n"
              << "  -s <peer_ip> <file1> ...     Send a file to peer\n"
              << "  -m <peer_ip> <message>       Send a message to peer\n"
              << "  -r                           Receive a file\n"
              << "  -h                           Show this help message\n";
}