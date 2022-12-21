#include "bencode.h"

BencodeType BencodeValue::getType() {
    return type;
}

unsigned char* BencodeValue::getSHA1() {
    return SHA1;
}