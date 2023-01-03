#include "torrent.h"
#include <iostream>

TorrentMetadata::TorrentMetadata(BencodeFile *file) {
    BencodeValue *value = file->nextValue();
    BencodeDict *dict = dynamic_cast<BencodeDict*>(value);
    if (dict == nullptr) {
        throw "Bencode file doesn't begin with a dictonary";
    }

    BencodeValue *bencodeAnnounce = dynamic_cast<BencodeString*>(dict->getValue(ANNOUNCE_KEY));
    if (bencodeAnnounce == nullptr) {
        throw "Bencode announce doesn't exist";
    }
    announce = bencodeAnnounce->toString();

    BencodeDict *info = dynamic_cast<BencodeDict*>(dict->getValue(INFO_KEY));
    if (info == nullptr) {
        throw "Bencode info doesn't exist";
    }
    unsigned char *infoSHA1 = info->getSHA1();
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        infoHash[i] = infoSHA1[i];
    }

    BencodeString *bencodeName = dynamic_cast<BencodeString*>(info->getValue(NAME_KEY));
    if (bencodeName == nullptr) {
        throw "Bencode name doesn't exist";
    }
    name = bencodeName->toString();
    
    BencodeNumber *bencodePieceLength = dynamic_cast<BencodeNumber*>(info->getValue(PIECE_LENGTH_KEY));
    if (bencodePieceLength == nullptr) {
        throw "Bencode pieve length doesn't exist";
    }
    pieceLength = bencodePieceLength->asLong();

    BencodeNumber *bencodeFileLength = dynamic_cast<BencodeNumber*>(info->getValue(LENGTH_KEY));
    if (bencodeFileLength == nullptr) {
        throw "Bencode pieve length doesn't exist";
    }
    fileLength = bencodeFileLength->asLong();

    BencodeString *bencodePieces = dynamic_cast<BencodeString*>(info->getValue(PIECES_KEY));
    if (bencodePieces == nullptr) {
        throw "Bencode pieces doesn't exist";
    }
    pieces = bencodePieces->asVector();

    std::cout << SHA_DIGEST_LENGTH << std::endl;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        printf("%02X", info->getSHA1()[i]);
    }
    std::cout << std::endl;

    delete value;
}

const std::string& TorrentMetadata::getAnnounce() {
    return announce;
}

unsigned char* TorrentMetadata::getInfoHash() {
    return infoHash;
}

void TorrentMetadata::announceUrlComponents(struct UrlComponents *annouceUrl) {
    int firstColon = announce.find(':');
    int secondColon = announce.find(':', firstColon + 1);
    int endPort = secondColon + 1;
    for (; endPort < announce.length() && isdigit(announce[endPort]); endPort++);

    annouceUrl->protocol = announce.substr(0, firstColon);
    annouceUrl->hostname = announce.substr(firstColon + 3, secondColon - (firstColon + 3)); // Plus 1 for the colon and 2 for the double slash
    annouceUrl->port = announce.substr(secondColon + 1, endPort - (secondColon + 1));
    annouceUrl->resource = announce.substr(endPort);
}

const std::string& TorrentMetadata::getName() {
    return name;
}

long TorrentMetadata::getPieceLength() {
    return pieceLength;
}

long TorrentMetadata::getFileLength() {
    return fileLength;
}

const std::vector<char>& TorrentMetadata::getPieces() {
    return pieces;
}

