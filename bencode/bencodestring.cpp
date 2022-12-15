#include "bencode.h"

void BencodeString::addByte(char c) {
    value.push_back(c);
}

std::vector<char> BencodeString::asVector() const {
    return value;
}