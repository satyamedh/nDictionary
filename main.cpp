#include <os.h>
#include <libndls.h>
#include "dictionary.h"
#include <string>
#include <vector>
#include <cstring>

void clearScreen() {
    // Screen clearing disabled
}

void printMessage(const char* msg) {
    puts(msg);
}

void printMeanings(const std::vector<std::string>& meanings) {
    if (meanings.empty()) {
        printMessage("Word not found in dictionary.");
        return;
    }

    printMessage("Meanings:");
    for (size_t i = 0; i < meanings.size(); i++) {
        printf("%d. %s\n", (int)(i + 1), meanings[i].c_str());
    }
}

std::string getInputWord() {
    char buffer[128];
    printMessage("\nEnter word to lookup (or '.' to exit): ");

    // Clear input buffer first
    fflush(stdin);

    // Use scanf instead of fgets to avoid double input issue
    if (scanf("%127s", buffer) == 1) {
        return std::string(buffer);
    }

    return std::string("");
}

int main(void) {

    // patch first column of the LCD (Fixing TI's bugs for them :D)
    if (is_cx2 && *(volatile unsigned int*)0xC0000004 == 0x03780d3f)
        *(volatile unsigned int *)0xC0000004 = 0x0720013F;



    printMessage("nDictionary - TI-Nspire Dictionary Lookup");
    printMessage("Created by Satyamedh Hulyalkar | https://github.com/satyamedh");
    printMessage("========================================");

    Dictionary dict;

    // Initialize dictionary with file paths
    // Assuming files are in the same directory as the executable
    if (!dict.initialize("index.csv.tns", "huffman_table.csv.tns", "dictionary.bin.tns")) {
        printMessage("Error: Could not load dictionary files!");
        printMessage("Make sure index.csv.tns, huffman_table.csv.tns, and dictionary.bin.tns");
        printMessage("are in the root directory of your TI-Nspire.");
        printMessage("\nPress any key to exit...");
        wait_key_pressed();
        return 1;
    }

    printMessage("Dictionary loaded successfully!");

    while (true) {
        std::string word = getInputWord();

        if (word == ".") {
            break;
        }

        if (word.empty()) {
            continue;
        }

        printMessage("Looking up word...");
        std::vector<std::string> meanings = dict.lookup(word);

        printf("\nWord: %s\n", word.c_str());
        printMessage("==================");
        printMeanings(meanings);
    }

    dict.cleanup();
    printMessage("Thank you for using nDictionary! Press any key to exit...");
    wait_key_pressed();
    return 0;
}
