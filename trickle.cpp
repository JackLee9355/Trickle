#include <iostream>
#include "torrent.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Wrong number of arguments." << std::endl;
        return 1;
    }

    Torrent *torrent = new Torrent(argv[1]);
    delete torrent;
    return 0;
}