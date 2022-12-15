#include "bencode.h"

bool BencodeStringCompare::operator() (const BencodeString& lhs, const BencodeString& rhs) const {
    return lhs.asVector() < rhs.asVector();
}

void BencodeDict::addEntry(BencodeString key, BencodeValue value) {
    values.insert({key, value});
}

std::map<BencodeString, BencodeValue, BencodeStringCompare> BencodeDict::asMap() {
    return values;
}