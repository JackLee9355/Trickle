#include "bencode.h"

void BencodeList::add(BencodeValue value) {
    values.push_back(value);
}

std::vector<BencodeValue> BencodeList::asVector() {
    return values;
}