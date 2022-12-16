#include <iostream>
#include "torrent/torrent.h"
#include "bencode/bencode.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Wrong number of arguments." << std::endl;
        return 1;
    }

    BencodeFile bencode(argv[1]);
    try {
        BencodeValue* value = bencode.nextValue();
        BencodeDict* dict = dynamic_cast<BencodeDict*>(value);
        if (dict == nullptr) {
            std::cout << "Bencode file doesn't begin with a dictonary" << std::endl;
            return 1;
        }
        BencodeValue *annouce = dict->getValue("announce");
        if (annouce == nullptr) {
            std::cout << "Bencode announce doesn't exist" << std::endl;
            return 1;
        }
        std::cout << annouce->toString() << std::endl;
        delete value;
    } catch (const char* err) {
        std::cout << err << std::endl;
    }
    return 0;
}