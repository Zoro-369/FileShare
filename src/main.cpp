#include "FileSender.h"
#include "FileReceiver.h"
#include "PeerDiscovery.h"
#include "utils.h"


std::mutex coutMutex;



int main(int argc, char* argv[]) {
    int opt;
    std::string filename;
    std::string peerIP;
    std::string message;
    std::ostringstream oss;

    while ((opt = getopt(argc, argv, "ds:rhm:")) != -1) {
        switch (opt) {
            case 'd':
                std::cout << "Discovering peers...\n";
                {
                    PeerDiscovery discovery;
                    discovery.startDiscovery();
                }
                break;
            case 's':
                if (optind >= argc) {  // Ensure both peer IP and filename are provided
                    std::cerr << "Error: Missing arguments for -s <peer_ip> <filename>\n";
                    printUsage();
                    return 1;
                }
                peerIP = optarg;
                // Ensure there's another argument for the message
                if (optind + 1 > argc) {
                    std::cerr << "Error: Missing message argument.\n";
                    printUsage();
                    return 1;
                }

                for(int i = optind;i<argc;++i){
                    filename = argv[i];  // Next argument after peerIP
                    std::cout << "Sending file '" << filename << "' to " << peerIP << "\n";
                    {
                        FileSender sender(peerIP,filename);
                        sender.sendFile();
                    }
                }
                
                break;
            case 'r':
                std::cout << "Receiving file...\n";
                {
                    FileReceiver receiver;
                    receiver.startServer();
                }
                break;
            
            case 'm':
                if (optind >= argc) {  
                    std::cerr << "Error: Missing arguments for -m <peer_ip> <message>\n";
                    printUsage();
                    return 1;
                }

                peerIP = optarg;

                // Ensure there's another argument for the message
                if (optind + 1 > argc) {
                    std::cerr << "Error: Missing message argument.\n";
                    printUsage();
                    return 1;
                }
                // Join all remaining arguments into a single message
                for (int i = optind; i < argc; ++i) {
                    if (i > optind) oss << " ";  // Add space between wordsx
                    oss << argv[i];
                }
                message = oss.str();


                std::cout << "Sending message to " << peerIP << ": " << message << "\n";

                {
                    FileSender peer(peerIP, message);
                    peer.sendMessage();
                }
                break;

            case 'h':
                printUsage();
                return 0;
            default:
                printUsage();
                return 1;
        }
    }

    return 0;
}
