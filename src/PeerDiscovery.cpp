#include "PeerDiscovery.h"


#define DISCOVERY_PORT 9999
#define BROADCAST_IP "255.255.255.255"

std::string PeerDiscovery::getLocalIPAddress() {
    struct ifaddrs *ifAddrStruct = nullptr, *ifa = nullptr;
    void *tmpAddrPtr = nullptr;
    std::string localIP = "";

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;
        if (ifa->ifa_addr->sa_family == AF_INET) {  // Check for IPv4
            tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (strcmp(ifa->ifa_name, "lo") != 0) {  // Exclude loopback (127.0.0.1)
                localIP = addressBuffer;
                break;
            }
        }
    }

    if (ifAddrStruct) freeifaddrs(ifAddrStruct);
    return localIP;
}



void PeerDiscovery::broadcastPresence() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return;

    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        std::cerr << "Failed to enable broadcast\n";
        close(sock);
        return;
    }

    struct sockaddr_in broadcastAddr{};
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(DISCOVERY_PORT);
    broadcastAddr.sin_addr.s_addr = inet_addr(BROADCAST_IP);

    const char* message = "FILESHARE_DISCOVERY";

    auto startTime = std::chrono::steady_clock::now();  // Start time

    while (true) {
        sendto(sock, message, strlen(message), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        // std::cout << "Broadcasting presence...\n";
        sleep(2);  // Wait 2 seconds before next broadcast

        // Check timeout (10 seconds)
        auto elapsedTime = std::chrono::steady_clock::now() - startTime;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsedTime).count() >= 10) {
            std::cout << "Broadcasting stopped after timeout.\n";
            break;
        }
    }

    close(sock);
}




void PeerDiscovery::listenForPeers() {
    std::cout << "Listening for peers..." << std::endl;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    // Enable SO_REUSEADDR to allow multiple listeners on the same port
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set SO_REUSEADDR\n";
        close(sock);
        return;
    }

    // Set timeout for receiving data (10 seconds)
    struct timeval timeout;
    timeout.tv_sec = 10;  // Stop listening after 10 seconds
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in listenAddr{};
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(DISCOVERY_PORT);
    listenAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&listenAddr, sizeof(listenAddr)) < 0) {
        std::cerr << "Bind failed! Error: " << strerror(errno) << "\n";
        close(sock);
        return;
    }

    char buffer[1024];
    struct sockaddr_in senderAddr{};
    socklen_t senderLen = sizeof(senderAddr);

    std::string localIP = getLocalIPAddress();
    std::cout << "Local IP: " << localIP << std::endl;

    std::set<std::string> discoveredPeers;  // Store unique peer IPs

    while (true) {
        int bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &senderLen);

        if (bytesReceived < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                std::cout << "No more peers discovered within timeout. Stopping discovery.\n";
                break;  // Exit after timeout
            } else {
                std::cerr << "Error in recvfrom(): " << strerror(errno) << "\n";
                break;
            }
        }

        buffer[bytesReceived] = '\0';
        std::string peerIP = inet_ntoa(senderAddr.sin_addr);

        if (peerIP != localIP && discoveredPeers.find(peerIP) == discoveredPeers.end()) {
            discoveredPeers.insert(peerIP);
            std::cout << "Discovered peer: " << peerIP << "\n";
        }
    }

    close(sock);
}


void PeerDiscovery::startDiscovery() {
    std::thread listenerThread(&PeerDiscovery::listenForPeers, this);
    std::thread broadcasterThread(&PeerDiscovery::broadcastPresence, this);

    listenerThread.join();
    broadcasterThread.join();
}
