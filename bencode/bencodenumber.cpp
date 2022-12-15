#include "bencode.h"

void BencodeNumber::setValue(long value) {
    value = value;
}

long BencodeNumber::asLong() {
    return value;
}