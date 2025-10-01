# nDictionary

A compressed dictionary application for the TI-Nspire calculator using Ndless. This project provides a complete offline dictionary with efficient storage using Huffman compression.
(Also, this was partially vibe-coded)


## Prerequisites

### For Building the Dictionary Data
- **Python 3.7+** with pip
- **Wiktextract data file** (wiktextract.jsonl)

### For Building the TI-Nspire Application
- **Ndless SDK** (nspire-gcc toolchain)
- **make** utility
- **Linux/WSL environment** (recommended)

## Installation

### 1. Set Up Python Environment

```bash
cd process_words
pip install -r requirements.txt
```

### 2. Download Dictionary Data

Download the English wiktextract JSONL file from [Kaikki.org](https://kaikki.org/dictionary/rawdata.html):

```bash
# Example: Download English wiktextract data
wget https://kaikki.org/dictionary/English/kaikki.org-dictionary-English.jsonl
mv kaikki.org-dictionary-English.jsonl process_words/wiktextract.jsonl
```

### 3. Set Environment Variable

```bash
export WIKTEXTRACT_PATH="process_words/wiktextract.jsonl"
```

### 4. Build Everything

From the main project directory:

```bash
make
```


This will:
1. Process the wiktextract data to generate compressed dictionary files
2. Compile the C++ application for TI-Nspire
3. Copy all files to the `calcbin/` folder


### 5. Copy to Calculator
Copy the generated files from `calcbin/` to your TI-Nspire calculator. 
huffman_table.csv.tns, index.csv.tns, and dictionary.bin.tns must be in the root directory of the calculator, but nDictionary.tns can be in any folder.

## File Formats

The dictionary uses three main files:

### `dictionary.bin.tns`
Binary file containing Huffman-encoded word definitions:
- `uint16`: Size of encoded data (big-endian)
- `uint8`: Number of padding bits
- `bytes[]`: Huffman-encoded text

### `index.csv.tns`
CSV index for word lookups:
```csv
word,offset,size
apple,0,156
banana,156,203
...
```

### `huffman_table.csv.tns`
Character encoding table:
```csv
char,code
32,101
97,001
101,11
...
```

## Usage

1. Copy files from `calcbin/` to your TI-Nspire calculator. huffman_table.csv.tns, index.csv.tns, and dictionary.bin.tns must be in the root directory of the calculator.
2. Launch `nDictionary.tns` using Ndless
3. Type a word and press Enter to search
4. Use DEL key to delete characters
5. Press ESC or use calculator controls to exit

## Troubleshooting

### "JSONL file not found" Error
Ensure the wiktextract file exists and set the environment variable:
```bash
export WIKTEXTRACT_PATH="/full/path/to/wiktextract.jsonl"
```

### Python Dependencies Missing
Install required packages:
```bash
pip install -r process_words/requirements.txt
```

### Ndless Compilation Errors
Ensure you have the Ndless SDK properly installed and in your PATH:
```bash
which nspire-gcc  # Should return a path
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on actual TI-Nspire hardware
5. Submit a pull request

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.



