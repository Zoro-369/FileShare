#include "FileSender.h"
#include "utils.h"

#define PORT "8080"
#define BUFFER_SIZE 1024

FileSender::FileSender(const std::string& serverHost, const std::string& fileName) 
    : serverHost(serverHost), fileName(fileName), clientSocket(-1) {}

FileSender::~FileSender() {
    if (clientSocket != -1) close(clientSocket);
}


bool FileSender::connectToServer(){
    struct addrinfo hints{}, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(serverHost.c_str(), PORT, &hints, &res) != 0) {
        std::cerr << "getaddrinfo() failed.\n";
        return false;
    }

    for (p = res; p != nullptr; p = p->ai_next) {
        clientSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (clientSocket == -1) continue;
        if (connect(clientSocket, p->ai_addr, p->ai_addrlen) == 0) break;
        close(clientSocket);
    }


    freeaddrinfo(res);
    return p != nullptr;
}

void FileSender::sendFileName() {
    uint32_t nameLen = htonl(fileName.length()); // Convert to network byte order and uint32_t ensures it is 32 bits.

    send(clientSocket, &nameLen, sizeof(nameLen), 0);// Send length first
    send(clientSocket, fileName.c_str(), fileName.length(), 0); // Send actual file name
}

void FileSender::sendFile() {
    if (!connectToServer()) {
        std::cerr << "Connection failed.\n";
        return;
    }
    uint8_t type = 0x01;
    send(clientSocket, &type, 1, 0);
    sendFileName();
    std::ifstream inFile(fileName, std::ios::binary | std::ios::ate);
    if (!inFile) {
        std::cerr << "Error opening file.\n";
        return;
    }

    uint32_t totalSize = inFile.tellg();
    totalSize = htonl(totalSize);  // Convert to network byte order
    inFile.seekg(0, std::ios::beg);

    // Send file size before sending the file content
    if (send(clientSocket, &totalSize, sizeof(totalSize), 0) <= 0) {
        std::cerr << "Failed to send file size.\n";
        return;
    }
    
    char buffer[BUFFER_SIZE];
    size_t sentBytes = 0;
    uint32_t totalSizeHost = ntohl(totalSize);  // Convert back for display

    while (sentBytes < totalSizeHost) {
        inFile.read(buffer, BUFFER_SIZE);
        size_t bytesRead = inFile.gcount();
        if (bytesRead == 0) break;
        
        if (send(clientSocket, buffer, bytesRead, 0) <= 0) {
            std::cerr << "Error sending file data.\n";
            return;
        }
        sentBytes += bytesRead;
        printProgressBar(sentBytes, totalSize, 20);
    }

    std::cout << "\r[####################] 100% \nFile sent successfully.\n";
    inFile.close();
}

void FileSender::sendMessage() {
    if (!connectToServer()) {
        std::cerr << "Connection failed.\n";
        return;
    }
    std::string peerIP = serverHost;
    std::string message  = fileName;

    uint8_t type = 0x02;  // Message type
    uint32_t length = htonl(message.size());

    send(clientSocket, &type, 1, 0);
    send(clientSocket, &length, sizeof(length), 0);
    send(clientSocket, message.c_str(), message.size(), 0);

    close(clientSocket);
}
