# Makefile for dictionary processing
# Processes wiktextract data and generates TI-Nspire dictionary files

# Variables
PYTHON = python3
SCRIPT = process_words.py
OUTPUT_FILES = dictionary.bin.tns index.csv.tns huffman_table.csv.tns

# Default target
.PHONY: all clean stats

all: $(OUTPUT_FILES)

# Main processing target
$(OUTPUT_FILES): $(SCRIPT)
	@echo "Processing dictionary data..."
	$(PYTHON) $(SCRIPT)
	@echo "Dictionary files generated successfully!"

# Clean generated files
clean:
	@echo "Cleaning generated files..."
	rm -f $(OUTPUT_FILES)
	rm -f *.pyc __pycache__/*
	@echo "Clean complete."

# Show file sizes and statistics
stats: $(OUTPUT_FILES)
	@echo "Dictionary file statistics:"
	@echo "=========================="
	@for file in $(OUTPUT_FILES); do \
		if [ -f "$$file" ]; then \
			size=$$(stat -c%s "$$file" 2>/dev/null || stat -f%z "$$file" 2>/dev/null); \
			echo "$$file: $$(numfmt --to=iec $$size)"; \
		fi \
	done
	@if [ -f "index.csv.tns" ]; then \
		echo "Word count: $$(tail -n +2 index.csv.tns | wc -l)"; \
	fi
