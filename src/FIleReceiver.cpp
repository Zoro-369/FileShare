#include "FileReceiver.h"
#include "utils.h"


#define PORT "8080"  
#define BUFFER_SIZE 1024
#define BACKLOG 5 


void FileReceiver::startServer(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating server socket.\n";
        return;
    }

    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Binding failed.\n";
        return;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Listening failed.\n";
        return;
    }

    std::cout << "Waiting for incoming file transfers...\n";

    while (true) {
        struct sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept connection.\n";
            continue;
        }

        receiveData(clientSocket);
        close(clientSocket);
    }

    close(serverSocket);
}

std::string FileReceiver::receiveFileName(int clientSocket) {
    uint32_t nameLen;
    if(recv(clientSocket, &nameLen, sizeof(nameLen), MSG_WAITALL)<= 0){
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr<<"Failed to recieve file name length. \n";
        return "";
    }
    nameLen = ntohl(nameLen);

    if (nameLen <= 0 || nameLen > 1024) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Invalid file name length.\n";
        return "";
    }

    char* buffer = new char[nameLen + 1];
    int recieved = recv(clientSocket, buffer, nameLen, 0);
    if(recieved <= 0){
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr<<"Failed to recieve file name.\n";
        delete[] buffer;
        return "";
    }
    buffer[recieved] = '\0';

    std::string fileName(buffer);
    delete[] buffer;

    // Lock and print file name properly
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "\nReceiving file: " << fileName << std::endl;
    }

    return fileName;
}

void FileReceiver::receiveFile(int clientSocket,std::string filename) {
    std::string fileName = filename;
    if (fileName.empty()) return;

    uint32_t fileSize;
    if (recv(clientSocket, &fileSize, sizeof(fileSize), MSG_WAITALL) <= 0) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Failed to receive file size.\n";
        return;
    }
    fileSize = ntohl(fileSize);

    char buffer[BUFFER_SIZE];
    
    
    std::string outputPath = "received_files/" + fileName;
    
    std::ofstream file;
    uint32_t offset = 0;
    // If file exists, get current size and open in append mode
    struct stat st;
    if(stat(outputPath.c_str(),&st) == 0){
        offset = st.st_size;
        file.open(outputPath,std::ios::binary | std::ios::app);
    }
    else{
        file.open(outputPath,std::ios::binary);
    }
    if (!file) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Error opening file for writing.\n";
        return;
    }
    // Send offset to sender
    uint32_t offsetNet = htobe32(offset);
    send(clientSocket,&offsetNet,sizeof(offsetNet),0);

    // Start Receiving from offset
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Receiving file: " << fileName << " (" << fileSize << " bytes)\n";
    }

    ssize_t bytesReceived;
    uint32_t receivedBytes = offset;
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout<<"Starting receiving file from "<<receivedBytes<<" Bytes"<<std::endl;
    }
    while (receivedBytes<fileSize) {
        bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if(bytesReceived <= 0){
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr<<"Connection lost at "<<receivedBytes<<"/"<<fileSize<<std::endl;
            break;
        }
        file.write(buffer, bytesReceived);
        // outFile.flush();

        receivedBytes += bytesReceived;

        // Print progress bar safely
        printProgressBar(receivedBytes, fileSize,20);
    }
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        if (receivedBytes != fileSize) {
            std::cout<<std::endl;
            std::cerr << "Warning: Expected " << fileSize << " bytes but received " << receivedBytes<< " bytes!\n";
        }
    }
    

      
    if(receivedBytes == fileSize){
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "\r[####################] 100% \nFile received successfully.\n";
    }
        
    file.close();
}

void FileReceiver::receiveData(int clientSocket) {
    while (true) {
        uint8_t type;
        if (recv(clientSocket, &type, 1, 0) <= 0) {
            std::cerr << "Connection closed.\n";
            break;
        }

        uint32_t dataLength;
        if (recv(clientSocket, &dataLength, sizeof(dataLength), 0) <= 0) {
            std::cerr << "Failed to read message length.\n";
            break;
        }
        dataLength = ntohl(dataLength);

        std::vector<char> buffer(dataLength);
        if (recv(clientSocket, buffer.data(), dataLength, 0) <= 0) {
            std::cerr << "Failed to read message data.\n";
            break;
        }

        if (type == 0x01) {
            std::cout << "Receiving a file...\n";
            receiveFile(clientSocket, buffer.data());
        } else if (type == 0x02) {
            std::cout << "Message from peer: " << std::string(buffer.begin(), buffer.end()) << "\n";
        }
    }
}
