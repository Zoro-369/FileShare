#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

class FileSender{
    public:
        FileSender(const std::string & serverHost,const std::string & fileName);
        ~FileSender();
        bool connectToServer();
        void sendFile();
        void sendFileName();
        void sendMessage();

    private:
        std::string serverHost;
        std::string fileName;
        int clientSocket;
        
};