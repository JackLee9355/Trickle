#ifndef TORRENT_H
#define TORRENT_H

#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

class Torrent {
  private:
    std::string path;
    std::string content;

  public:
    Torrent(const std::string path);

  private:
    void loadFromPath();
};

#endif