#include "torrent.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <cstring>
#include <random>


Torrent::Torrent(const std::string& path) {
    BencodeFile bencode(path);
    metadata = new TorrentMetadata(&bencode);
    initPeerId();
    announce();
}

Torrent::~Torrent() {
    delete metadata;
}

void Torrent::initPeerId() {
    std::random_device rd;  
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned char> distribution(0, 127);

    for (int i = 0; i < PEER_ID_LENGTH; i++) {
        peerId[i] = distribution(gen);
    }
}

int Torrent::announce() {
    struct UrlComponents url;
    metadata->announceUrlComponents(&url);
    int socketType = url.protocol == "udp" ? SOCK_DGRAM : SOCK_STREAM; // Currently unused. Probably should be.

    struct addrinfo hints;
    struct addrinfo *result, *rp;

    // This may or may not be copied from the manpages... It's a real PITA
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0; // Any protocol, should probably be switched to udp sometimes?
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    std::cout << url.hostname.c_str() << std::endl;
    int err = getaddrinfo(url.hostname.c_str(), NULL, &hints, &result);
    if (err < 0) {
        std::cout << err << std::endl;
        throw "Error occurred while attempting to resolve domain name";
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        int sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            std::cout << "Error creating socket" << std::endl;
            continue;
        }

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) < 0) {
            close(sfd);
            throw "Error connecting to socket";
        }

        close(sfd);
    }

    freeaddrinfo(result);
    return 0;
}