#include "bencode.h"
#include <iostream>

BencodeNumber::BencodeNumber() {
    type = bencodeNumber;
}

BencodeNumber::~BencodeNumber() {}

void BencodeNumber::setValue(long newValue) {
    value = newValue;
}

long BencodeNumber::asLong() {
    return value;
}

std::string BencodeNumber::toString() const {
    return std::to_string(value);
}