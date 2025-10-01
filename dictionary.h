#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <os.h>
#include <string>
#include <vector>
#include <map>

// Structure to hold index entry
struct IndexEntry {
    std::string word;
    uint32_t offset;
    uint16_t size;
};

// Huffman decoder class - memory efficient
class HuffmanDecoder {
private:
    std::map<std::string, char> codeToChar;

public:
    bool loadTable(const char* filename);
    std::string decode(const uint8_t* data, uint16_t size, uint8_t padding);
};

// Dictionary lookup class
class Dictionary {
private:
    std::vector<IndexEntry> index;
    HuffmanDecoder huffman;
    std::string binaryFilename;

    bool loadIndex(const char* indexFile);
    IndexEntry* findWord(const std::string& word);

public:
    bool initialize(const char* indexFile, const char* huffmanFile, const char* binaryFile);
    std::vector<std::string> lookup(const std::string& word);
    void cleanup();
};

#endif
