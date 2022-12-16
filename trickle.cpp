#include <iostream>
#include "torrent/torrent.h"
#include "bencode/bencode.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Wrong number of arguments." << std::endl;
        return 1;
    }

    try {
        BencodeFile bencode(argv[1]);
        Torrent torrent(&bencode);
    } catch (const char* err) {
        std::cout << err << std::endl;
    }
    return 0;
}