#include <iostream>
#include "torrent/torrent.h"
#include "bencode/bencode.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Wrong number of arguments." << std::endl;
        return 1;
    }

    BencodeFile *bencode = new BencodeFile(argv[1]);
    try {
        BencodeValue* dict = bencode->nextValue();
        std::cout << dict->toString() << std::endl;
        delete dict;
    } catch (const char* err) {
        std::cout << err << std::endl;
    }
    delete bencode;
    return 0;
}