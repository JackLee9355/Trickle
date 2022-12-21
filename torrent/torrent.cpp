#include "torrent.h"
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/sha.h>


Torrent::Torrent(const std::string& path) {
    BencodeFile bencode(path);
    metadata = new TorrentMetadata(&bencode);
}

Torrent::~Torrent() {
    delete metadata;
}

int Torrent::announce() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw "Error creating announcer socket.";
    }

    struct sockaddr_in annouceAddr;
    return fd;
}