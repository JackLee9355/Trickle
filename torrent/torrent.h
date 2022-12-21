#ifndef TORRENT_H
#define TORRENT_H

#include <bencode/bencode.h>
#include <string>
#include <vector>

const std::string ANNOUNCE_KEY = "announce";
const std::string INFO_KEY = "info";
const std::string PIECE_LENGTH_KEY = "piece length";
const std::string NAME_KEY = "name";
const std::string PIECES_KEY = "pieces";

class TorrentMetadata {
  private:
    std::string announce;
    std::string name;
    long pieceLength;
    std::vector<char> pieces;

  public:
    TorrentMetadata(BencodeFile *file);
    const std::string& getAnnounce();
    const std::string& getName();
    long getPieceLength();
    const std::vector<char>& getPieces();
};

class Torrent {
  private:
    TorrentMetadata *metadata;

  public:
    Torrent(const std::string& path);
    ~Torrent();

  private:
    int announce();
};

#endif