#ifndef PEERDISCOVERY_H
#define PEERDISCOVERY_H

#include <iostream>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <ifaddrs.h>
#include <set>
#include <chrono>

class PeerDiscovery {
public:
    void startDiscovery();
private:
    void broadcastPresence();
    void listenForPeers();
    std::string getLocalIPAddress();
};

#endif
