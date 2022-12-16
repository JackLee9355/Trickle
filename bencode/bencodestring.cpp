#include "bencode.h"

bool BencodeStringCompare::operator() (const BencodeString* lhs, const BencodeString* rhs) const {
    return lhs->asVector() < rhs->asVector();
}

BencodeString::BencodeString() {
    type = bencodeString;
}

BencodeString::BencodeString(const std::string& initVal) : value(initVal.begin(), initVal.end()) {
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
    return s;
}