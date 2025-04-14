#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <mutex>
#include <arpa/inet.h>
#include <filesystem>
#include <unistd.h> // For close()
#include<vector>
#include<sys/stat.h>

class FileReceiver {
public:
    void receiveFile(int clientSocket,std::string filename);
    void startServer();
    void receiveData(int clientSocket);
private:
    std::string receiveFileName(int clientSocket);
};


