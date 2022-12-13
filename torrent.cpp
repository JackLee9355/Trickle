#include "torrent.h"

Torrent::Torrent(const std::string path) : path(std::move(path)) {
    loadFromPath();
}

void Torrent::loadFromPath() {
    std::ifstream file(path);
    std::stringstream strStream;
    strStream << file.rdbuf();
    content = strStream.str();
    std::cout << content << std::endl;
}