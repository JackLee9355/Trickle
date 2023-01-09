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

long Torrent::getLeft() {
    // If we add reinitialization this will need to be changed.
    return metadata->getFileLength() - downloaded;
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

    struct addrinfo *result, *rp;

    std::cout << url.hostname.c_str() << " " << url.port.c_str() << std::endl;
    int err = getaddrinfo(url.hostname.c_str(), url.port.c_str(), NULL, &result);
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
            std::cout << "Error connecting to socket" << std::endl;
            continue;
        }

        if (send(sfd, request.c_str(), strlen(request.c_str()), 0) < 0) {
            close(sfd);
            std::cout << "Error writing to socket" << std::endl;
            continue;
        }

        long bufLength = 512;
        long used = 0;
        char *buf = new char[bufLength];
        long amountRead;
        while ((amountRead = read(sfd, buf + used, bufLength - used)) > 0) {
            if (amountRead < bufLength - used) {
                used += amountRead;
                break;
            }

            used += amountRead;
            char *tmp = new char[bufLength * 2];
            for (long i = 0; i < bufLength - used; i++) {
                tmp[i] = buf[i];
            }
            delete[] buf;
            buf = tmp;
            bufLength *= 2;
        } 

        if (amountRead < 0) {
            close(sfd);
            std::cout << "Error reading from socket" << std::endl;
            continue;
        }

        if (strncmp(buf, "HTTP/1.1 200 OK", strlen("HTTP/1.1 200 OK")) != 0) {
            close(sfd);
            std::cout << "Did not receive HTTP/1.1 200 OK from socket" << std::endl;
            continue;
        }

        char *body = strstr(buf, "\r\n\r\n");
        if (body == nullptr) {
            close(sfd);
            std::cout << "HTTP response has no body" << std::endl;
            continue;
        }
        body += strlen("\r\n\r\n");
        BencodeFile bencodeFile(body, used - (buf - body));
        delete[] buf;
        
        BencodeValue *value = bencodeFile.nextValue();
        std::cout << value->toString() << std::endl;
        delete value;
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
    request += "&left=" + std::to_string(getLeft());
    request += "&compact=1"; // This seems to be the defacto standard these days. And I only want to implement one lol
    request += " HTTP/1.1\r\n";
    request += "Host: " + url->hostname + ":" + url->port + "\r\n\r\n";
    std::cout << request;
    return request;
}