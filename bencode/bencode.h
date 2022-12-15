#ifndef BENCODE_H
#define BENCODE_H

#include <string>
#include <vector>
#include <map>

const char BENCODE_END = 'e';
const char BENCODE_SEPERATOR = ':';

enum BencodeType {
    bencodeDict = 'd',
    bencodeList = 'l',
    bencodeNumber = 'i',
    bencodeString,
};

class BencodeValue {
  private:
    BencodeType type;

  public:
    BencodeType getType();
};

class BencodeNumber : public BencodeValue {
  private:
    long value;

  public:
    void setValue(long value);
    long asLong();
};

class BencodeString : public BencodeValue {
  private:
    std::vector<char> value;

  public:
    void addByte(char c);
    std::vector<char> asVector() const;

};

struct BencodeStringCompare {
  bool operator() (const BencodeString& lhs, const BencodeString& rhs) const;
};

class BencodeList : public BencodeValue {
  private:
    std::vector<BencodeValue> values;

  public:
    void add(BencodeValue value);
    std::vector<BencodeValue> asVector();
};

class BencodeDict : public BencodeValue {
  private:
    std::map<BencodeString, BencodeValue, BencodeStringCompare> values;

  public:
    void addEntry(BencodeString key, BencodeValue value);
    std::map<BencodeString, BencodeValue, BencodeStringCompare> asMap();
};

class BencodeFile {
  private:
    std::string path;
    std::vector<char> content;
    long curLoc = 0;

  public:
    BencodeFile(const std::string path);
    BencodeValue nextValue();
    
  private:
    void loadFromPath();
    BencodeString nextString();
    BencodeList nextList();
    BencodeDict nextDict();
    BencodeNumber nextNumber();
    long nextLong();
    void skipEnd();
    void verifyLoc();
};

#endif // BENCODE_H