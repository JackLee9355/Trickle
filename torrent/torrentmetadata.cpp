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

    BencodeString *bencodeName = dynamic_cast<BencodeString*>(info->getValue(NAME_KEY));
    if (bencodeName == nullptr) {
        throw "Bencode name doesn't exist";
    }
    name = bencodeName->toString();
    
    BencodeNumber *bencodeLength = dynamic_cast<BencodeNumber*>(info->getValue(PIECE_LENGTH_KEY));
    if (bencodeLength == nullptr) {
        throw "Bencode pieve length doesn't exist";
    }
    pieceLength = bencodeLength->asLong();

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

const std::string& TorrentMetadata::getName() {
    return name;
}

long TorrentMetadata::getPieceLength() {
    return pieceLength;
}

const std::vector<char>& TorrentMetadata::getPieces() {
    return pieces;
}

