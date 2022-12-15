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

BencodeValue* BencodeFile::nextValue() {
    verifyLoc();
    char chr = content[curLoc];
    switch (chr) {
        case bencodeDict:
            return nextDict();
        case bencodeNumber:
            return nextNumber();
        case bencodeList:
            return nextList();
        default:
            if (isdigit(chr)) {
                return nextString();
            }
    }

    throw "Invalid bencode file. Current character is invalid for new value. Char: " + chr;
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

    return result;
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
    std::cout << value << std::endl;
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
        BencodeString *key = nextString();
        BencodeValue *value = nextValue();
        dict->addEntry(key, value);
    }
    skipEnd();
    return dict;
}