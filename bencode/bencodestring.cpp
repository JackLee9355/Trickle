#include "bencode.h"

bool BencodeStringCompare::operator() (const BencodeString* lhs, const BencodeString* rhs) const {
    return lhs->asVector() < rhs->asVector();
}

BencodeString::BencodeString() {
    type = bencodeString;
}

BencodeString::~BencodeString() {}

void BencodeString::addByte(char c) {
    value.push_back(c);
}

std::vector<char> BencodeString::asVector() const {
    return value;
}

std::string BencodeString::toString() const {
    std::string s(value.begin(), value.end());
    s.insert(0, 1, '"');
    s.push_back('"');
    return s;
}