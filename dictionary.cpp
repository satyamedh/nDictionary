#include "dictionary.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

// Helper function to read uint16 in big-endian format
uint16_t readBigEndianUint16(FILE* file) {
    uint8_t bytes[2];
    fread(bytes, 1, 2, file);
    return (bytes[0] << 8) | bytes[1];
}

// Helper function to split string by delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string current = "";

    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == delimiter) {
            if (!current.empty()) {
                result.push_back(current);
                current = "";
            }
        } else {
            current += str[i];
        }
    }

    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}

// HuffmanDecoder implementation
bool HuffmanDecoder::loadTable(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return false;

    char line[256];
    // Skip header
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        // Parse CSV line: char,code
        char* charStr = strtok(line, ",");
        char* codeStr = strtok(nullptr, ",\n\r");

        if (charStr && codeStr) {
            int charCode = atoi(charStr);
            codeToChar[std::string(codeStr)] = (char)charCode;
        }
    }

    fclose(file);
    return !codeToChar.empty();
}

std::string HuffmanDecoder::decode(const uint8_t* data, uint16_t size, uint8_t padding) {
    std::string result = "";
    std::string currentCode = "";

    // Convert bytes to bit string
    for (uint16_t i = 0; i < size; i++) {
        uint8_t byte = data[i];
        for (int bit = 7; bit >= 0; bit--) {
            // Skip padding bits on last byte
            if (i == size - 1 && bit < padding) break;

            currentCode += ((byte >> bit) & 1) ? '1' : '0';

            // Check if current code matches any character
            auto it = codeToChar.find(currentCode);
            if (it != codeToChar.end()) {
                result += it->second;
                currentCode = "";
            }
        }
    }

    return result;
}

// Dictionary implementation
bool Dictionary::loadIndex(const char* indexFile) {
    FILE* file = fopen(indexFile, "r");
    if (!file) return false;

    char line[512];
    // Skip header
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        // Parse CSV line: word,offset,size
        char* wordStr = strtok(line, ",");
        char* offsetStr = strtok(nullptr, ",");
        char* sizeStr = strtok(nullptr, ",\n\r");

        if (wordStr && offsetStr && sizeStr) {
            IndexEntry entry;
            entry.word = std::string(wordStr);
            entry.offset = strtoul(offsetStr, nullptr, 10);
            entry.size = (uint16_t)atoi(sizeStr);
            index.push_back(entry);
        }
    }

    fclose(file);
    return !index.empty();
}

IndexEntry* Dictionary::findWord(const std::string& word) {
    // Binary search since index is sorted by word
    int left = 0;
    int right = index.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (index[mid].word == word) {
            return &index[mid];
        }
        else if (index[mid].word < word) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return nullptr; // Word not found
}

bool Dictionary::initialize(const char* indexFile, const char* huffmanFile, const char* binaryFile) {
    binaryFilename = std::string(binaryFile);

    if (!loadIndex(indexFile)) {
        return false;
    }

    if (!huffman.loadTable(huffmanFile)) {
        return false;
    }

    return true;
}

std::vector<std::string> Dictionary::lookup(const std::string& word) {
    std::vector<std::string> meanings;

    IndexEntry* entry = findWord(word);
    if (!entry) {
        return meanings; // Word not found
    }

    FILE* file = fopen(binaryFilename.c_str(), "rb");
    if (!file) {
        return meanings;
    }

    // Seek to the word's data
    fseek(file, entry->offset, SEEK_SET);

    // Read size and padding
    uint16_t encodedSize = readBigEndianUint16(file);
    uint8_t padding;
    fread(&padding, 1, 1, file);

    // Read encoded data
    uint8_t* encodedData = new uint8_t[encodedSize];
    fread(encodedData, 1, encodedSize, file);
    fclose(file);

    // Decode using Huffman
    std::string decodedText = huffman.decode(encodedData, encodedSize, padding);
    delete[] encodedData;

    // Split meanings by newline
    meanings = split(decodedText, '\n');

    return meanings;
}

void Dictionary::cleanup() {
    index.clear();
}
