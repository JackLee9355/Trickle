#include "bencode.h"

BencodeDict::BencodeDict() {
    type = bencodeDict;
}

BencodeDict::~BencodeDict() {
    for (auto value : values) {
        delete value.first;
        delete value.second;
    }
}

void BencodeDict::addEntry(BencodeString *key, BencodeValue *value) {
    values.insert({key, value});
}

std::map<BencodeString*, BencodeValue*, BencodeStringCompare> BencodeDict::asMap() {
    return values;
}

std::string BencodeDict::toString() const {
    std::string ret = "{\n";
    for (auto value : values) {
        ret += value.first->toString() + ": " + value.second->toString() + ",\n";
    }
    ret += "}";
    return ret;
}