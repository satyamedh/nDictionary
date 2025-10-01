import json
import csv
import struct
from tqdm import tqdm
from collections import Counter
from heapq import heapify, heappush, heappop
from wordfreq import top_n_list
import os

# Get path from environment variable or use default
WIKTEXTRACT_PATH = os.getenv('WIKTEXTRACT_PATH')
if not WIKTEXTRACT_PATH or not os.path.isfile(WIKTEXTRACT_PATH):
    raise FileNotFoundError(f"JSONL file not found, please set WIKTEXTRACT_PATH environment variable correctly.")

def build_huffman_tree(frequencies):
    heap = [[weight, [char, ""]] for char, weight in frequencies.items()]
    heapify(heap)
    while len(heap) > 1:
        lo = heappop(heap)
        hi = heappop(heap)
        for pair in lo[1:]:
            pair[1] = '0' + pair[1]
        for pair in hi[1:]:
            pair[1] = '1' + pair[1]
        heappush(heap, [lo[0] + hi[0]] + lo[1:] + hi[1:])
    return dict(heap[0][1:])


def encode_text(text, codes):
    bitstring = ''.join(codes.get(c, '') for c in text)
    padding = (8 - len(bitstring) % 8) % 8
    bitstring = bitstring + '0' * padding
    return bytes(int(bitstring[i:i + 8], 2) for i in range(0, len(bitstring), 8)), padding


top_words = set(top_n_list('en', 500_000))
total_lines = 10_290_483

# First pass: collect character frequencies
char_freq = Counter()
with open(WIKTEXTRACT_PATH, encoding="utf-8") as f:
    for line in tqdm(f, total=total_lines, desc="Building frequency table"):
        data = json.loads(line)
        if data and "word" in data:
            word = data["word"].lower()
            if word.isascii() and word in top_words:
                for sense in data.get("senses", [])[:2]:
                    if "glosses" in sense and sense["glosses"]:
                        char_freq.update(sense["glosses"][0])

# Build and save Huffman table
huffman_codes = build_huffman_tree(char_freq)
with open("huffman_table.csv.tns", "w", encoding="utf-8", newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['char', 'code'])
    for char, code in sorted(huffman_codes.items()):
        writer.writerow([ord(char), code])


# Second pass: collect all entries first, then sort and remove duplicates
entries = []
with open(WIKTEXTRACT_PATH, encoding="utf-8") as f:
    for line in tqdm(f, total=total_lines, desc="Collecting entries"):
        data = json.loads(line)
        if not data or "word" not in data:
            continue

        word = data["word"].lower()
        if not word.isascii() or word not in top_words:
            continue

        meanings = []
        for sense in data.get("senses", [])[:2]:
            if "glosses" in sense and sense["glosses"]:
                meanings.append(sense["glosses"][0])

        if meanings:
            meanings.extend([''] * (2 - len(meanings)))
            entries.append((word, meanings))

# Sort and remove duplicates (keep first occurrence)
# Sort and choose best duplicates
word_entries = {}
for word, meanings in sorted(entries):
    # Filter out empty meanings
    meanings = [m for m in meanings if m]

    # Calculate entry quality score:
    # Number of meanings * total length of meanings
    score = len(meanings) * sum(len(m) for m in meanings)

    # Keep entry with highest score
    if word not in word_entries or score > word_entries[word][1]:
        word_entries[word] = (meanings, score)

# Convert to sorted list of (word, meanings)
sorted_entries = [
    (word, meanings_score[0])
    for word, meanings_score in sorted(word_entries.items())
]

# Write sorted entries to binary file and index
with open("dictionary.bin.tns", "wb") as bin_f, \
        open("index.csv.tns", "w", encoding="utf-8", newline='') as idx_f:

    idx_writer = csv.writer(idx_f)
    idx_writer.writerow(['word', 'offset', 'size'])

    for word, meanings in tqdm(sorted_entries, desc="Writing sorted entries"):
        offset = bin_f.tell()
        total_size = 0

        # Write both meanings in one block
        combined_text = '\n'.join(m for m in meanings if m)
        if combined_text:
            encoded_bytes, padding = encode_text(combined_text, huffman_codes)
            size = len(encoded_bytes)
            bin_f.write(struct.pack('!HB', size, padding))  # Using uint16 instead of uint32
            bin_f.write(encoded_bytes)
            total_size = 3 + size  # 2 bytes for size, 1 for padding
        else:
            bin_f.write(struct.pack('!HB', 0, 0))
            total_size = 3

        idx_writer.writerow([word, offset, total_size])