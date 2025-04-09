// utils.h
#ifndef UTILS_H
#define UTILS_H
#include <cstddef> // For size_t
#include <mutex> 
extern std::mutex coutMutex;
void printProgressBar(size_t completed, size_t total, int barWidth = 20);
void printUsage();

#endif