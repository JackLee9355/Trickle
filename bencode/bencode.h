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
  protected:
    BencodeType type;

  public:
    virtual ~BencodeValue() {}
    BencodeType getType();
    virtual std::string toString() const = 0;
};

class BencodeNumber : public BencodeValue {
  private:
    long value;

  public:
    BencodeNumber();
    virtual ~BencodeNumber();
    void setValue(long value);
    long asLong();
    virtual std::string toString() const;
};

class BencodeString : public BencodeValue {
  private:
    std::vector<char> value;

  public:
    BencodeString();
    virtual ~BencodeString();
    void addByte(char c);
    std::vector<char> asVector() const;
    virtual std::string toString() const;
};

struct BencodeStringCompare {
  bool operator() (const BencodeString* lhs, const BencodeString* rhs) const;
};

class BencodeList : public BencodeValue {
  private:
    std::vector<BencodeValue*> values;

  public:
    BencodeList();
    virtual ~BencodeList();
    void add(BencodeValue *value);
    std::vector<BencodeValue*> asVector();
    virtual std::string toString() const;
};

class BencodeDict : public BencodeValue {
  private:
    std::map<BencodeString*, BencodeValue*, BencodeStringCompare> values;

  public:
    BencodeDict();
    virtual ~BencodeDict();
    void addEntry(BencodeString *key, BencodeValue *value);
    std::map<BencodeString*, BencodeValue*, BencodeStringCompare> asMap();
    virtual std::string toString() const;
};

class BencodeFile {
  private:
    std::string path;
    std::vector<char> content;
    long curLoc = 0;

  public:
    BencodeFile(const std::string path);
    BencodeValue* nextValue();
    
  private:
    void loadFromPath();
    BencodeString* nextString();
    BencodeList* nextList();
    BencodeDict* nextDict();
    BencodeNumber* nextNumber();
    long nextLong();
    void skipEnd();
    void verifyLoc();
};

#endif // BENCODE_H