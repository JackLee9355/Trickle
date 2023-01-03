#ifndef TORRENT_H
#define TORRENT_H

#include <bencode/bencode.h>
#include <string>
#include <vector>
#include <openssl/sha.h>

const std::string ANNOUNCE_KEY = "announce";
const std::string INFO_KEY = "info";
const std::string LENGTH_KEY = "length";
const std::string PIECE_LENGTH_KEY = "piece length";
const std::string NAME_KEY = "name";
const std::string PIECES_KEY = "pieces";

const int PEER_ID_LENGTH = 20;

struct UrlComponents {
    std::string protocol;
    std::string hostname;
    std::string port;
    std::string resource;
};

class TorrentMetadata {
  private:
    std::string announce;
    std::string name;
    long pieceLength;
    long fileLength;
    std::vector<char> pieces;
    unsigned char infoHash[SHA_DIGEST_LENGTH];

  public:
    TorrentMetadata(BencodeFile *file);
    const std::string& getAnnounce();
    const std::string& getName();
    unsigned char* getInfoHash();
    long getFileLength();
    long getPieceLength();
    const std::vector<char>& getPieces();
    void announceUrlComponents(struct UrlComponents *annouceUrl);
};

class Torrent {
  private:
    TorrentMetadata *metadata;
    unsigned short port = 6881;
    long uploaded = 0;
    long downloaded = 0;
    unsigned char peerId[PEER_ID_LENGTH];

  public:
    Torrent(const std::string& path);
    ~Torrent();

  private:
    void initPeerId();
    int announce();
    long getLeft();
    std::string buildAnnouncerRequest(struct UrlComponents *url);
};

#endif