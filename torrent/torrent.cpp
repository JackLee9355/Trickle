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

    int socketType = url.protocol == "udp" ? SOCK_DGRAM : SOCK_STREAM; 
    // Currently unused. Probably should be.
    // Implementing UDP announcing is a whole deal. Will come back to maybe.
    if (url.protocol == "udp") {
        throw "UDP announcing unsupported at this time";
    }

    struct addrinfo hints;
    struct addrinfo *result, *rp;

    // This may or may not be copied from the manpages... It's a real PITA
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0; // Any protocol
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    std::cout << url.hostname.c_str() << " " << url.port.c_str() << std::endl;
    int err = getaddrinfo("google.com", "80", &hints, &result); // getaddrinfo(url.hostname.c_str(), url.port.c_str(), &hints, &result);
    if (err < 0) {
        std::cout << err << std::endl;
        throw "Error occurred while attempting to resolve domain name";
    }

    std::string request = buildAnnouncerRequest(&url);
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        int sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            std::cout << "Error creating socket" << std::endl;
            continue;
        }

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) < 0) {
            close(sfd);
            freeaddrinfo(result);
            throw "Error connecting to socket";
        }

        if (send(sfd, request.c_str(), strlen(request.c_str()), 0) < 0) {
            close(sfd);
            freeaddrinfo(result);
            throw "Error writing to socket";
        }

        char buf[2048];
        memset(buf, 0, sizeof(buf));
        std::cout << "Reading..." << std::endl;
        int amtRead = read(sfd, buf, 1);
        std::cout << buf << std::endl;

        close(sfd);
        break;
    }

    freeaddrinfo(result);
    return 0;
}

std::string percentEscaped(unsigned char *content, int length) {
    std::string escaped = "";
    for (int i = 0; i < length; i++) {
        char curEscaped[3];
        sprintf(curEscaped, "%02X", content[i]);
        escaped += '%';
        escaped += curEscaped[0];
        escaped += curEscaped[1];
    }
    return escaped;
}

std::string Torrent::buildAnnouncerRequest(struct UrlComponents *url) {
    std::string request = "GET " + url->resource;
    request += "?info_hash=" + percentEscaped(metadata->getInfoHash(), SHA_DIGEST_LENGTH);
    request += "&peer_id=" + percentEscaped(peerId, PEER_ID_LENGTH);
    request += "&port=" + std::to_string(port);
    request += "&uploaded=" + std::to_string(uploaded);
    request += "&downloaded=" + std::to_string(downloaded);
    request += "&left=" + std::to_string(1000); // TODO: Replace this with a actual value.
    request += " HTTP/1.1\r\n";
    request += "Host: " + url->hostname + ":" + url->port + "\r\n\r\n";

    request = "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
    std::cout << request << std::endl;
    return request;
}