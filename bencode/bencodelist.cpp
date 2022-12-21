#include "bencode.h"

BencodeList::BencodeList() {
    type = bencodeList;
}

BencodeList::~BencodeList() {
    for (auto value: values) {
        delete value;
    }
}

void BencodeList::add(BencodeValue *value) {
    values.push_back(value);
}

std::vector<BencodeValue*> BencodeList::asVector() {
    return values;
}

std::string BencodeList::toString() const {
    std::string ret = "{\n";
    for (auto value : values) {
        ret += value->toString() + ",\n";
    }
    ret += "}";
    return ret;
}