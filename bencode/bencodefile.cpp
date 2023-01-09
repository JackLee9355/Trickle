#include "bencode.h"
#include <iostream>
#include <fstream>

BencodeFile::BencodeFile(const std::string path) : path(std::move(path)) {
    loadFromPath();
}

void BencodeFile::loadFromPath() {
    content.clear();
    std::ifstream file(path);
    char current;
    while (file.good()) { 
        file.get(current);
        content.push_back(current);
    }
}

BencodeFile::BencodeFile(char* data, long length) {
    for (long i = 0; i < length; i++) {
        content.push_back(data[i]);
    }
}

BencodeValue* BencodeFile::nextValue() {
    verifyLoc();
    long start = curLoc; 
    char chr = content[curLoc];
    BencodeValue *next;
    switch (chr) {
        case bencodeDict:
            next = nextDict();
            break;
        case bencodeNumber:
            next = nextNumber();
            break;
        case bencodeList:
            next = nextList();
            break;
        default:
            if (isdigit(chr)) {
                next = nextString();
            } else {
                throw "Invalid bencode file. Current character is invalid for new value. Char: " + chr;
            }
            break;
    }

    long end = curLoc;
    calculateSHA1(start, end, next->getSHA1());
    return next;
}

void BencodeFile::verifyLoc() {
    if (curLoc > content.size()) {
        throw "Current location in content is greater than size of content.";
    }
}

void BencodeFile::skipEnd() {
    verifyLoc();
    if (content[curLoc] != BENCODE_END) {
        throw "Unexpected ending character. char: " + content[curLoc];
    }
    curLoc++;
}

long BencodeFile::nextLong() {
    char chr = content[curLoc++];
    bool isNeg = chr == '-';
    if (isNeg) {
        chr = content[curLoc++];
    }
    long result = chr - '0';

    while (isdigit(content[curLoc])) {
        chr = content[curLoc++];
        result = (result * 10) + (chr - '0');
    }

    return isNeg ? -result : result;
}

BencodeString* BencodeFile::nextString() {
    // std::cout << "String" << std::endl;
    BencodeString *str = new BencodeString;
    long length = nextLong();

    if (content[curLoc] != BENCODE_SEPERATOR) {
        throw "Unexpected character seperating BencodeString length from content: ";
    }
    curLoc++;

    for (int i = 0; i < length; i++) {
        str->addByte(content[curLoc++]);
    }
    return str;
}

BencodeNumber* BencodeFile::nextNumber() {
    // std::cout << "Number" << std::endl;
    curLoc++;
    BencodeNumber *number = new BencodeNumber;
    long value = nextLong();
    number->setValue(value);
    skipEnd();
    return number;
}

BencodeList* BencodeFile::nextList() {
    // std::cout << "List" << std::endl;
    curLoc++;
    BencodeList *list = new BencodeList;
    while (content[curLoc] != BENCODE_END) {
        list->add(nextValue());
    }
    skipEnd();
    return list;
}

BencodeDict* BencodeFile::nextDict() {
    // std::cout << "Dict" << std::endl;
    curLoc++;
    BencodeDict *dict = new BencodeDict;
    while (content[curLoc] != BENCODE_END) {
        BencodeString *key = dynamic_cast<BencodeString*>(nextValue());
        if (key == nullptr) {
            throw "Bencode dict expected a string for a key";
        }
        BencodeValue *value = nextValue();
        dict->addEntry(key, value);
    }
    skipEnd();
    return dict;
}

void BencodeFile::calculateSHA1(long start, long end, unsigned char *hash) {
    SHA_CTX ctx;
    SHA1_Init(&ctx);

    for (long i = start; i < end; i++) {
        SHA1_Update(&ctx, &content[i], sizeof(char));
    }

    SHA1_Final(hash, &ctx);
}