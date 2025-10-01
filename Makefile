DEBUG = FALSE

GCC = nspire-gcc
AS  = nspire-as
GXX = nspire-g++
LD  = nspire-ld
GENZEHN = genzehn

GCCFLAGS = -Wall -W -marm
LDFLAGS = -Wl,--nspireio
ZEHNFLAGS = --name "nDictionary"

ifeq ($(DEBUG),FALSE)
	GCCFLAGS += -Os
else
	GCCFLAGS += -O0 -g
endif

OBJS = $(patsubst %.c, %.o, $(shell find . -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find . -name \*.S))
EXE = nDictionary
DISTDIR = .
CALCBIN = calcbin
PROCESS_WORDS_DIR = process_words
vpath %.tns $(DISTDIR)
vpath %.elf $(DISTDIR)

all: process-words $(EXE).tns copy-to-calcbin

# Build dictionary data files using process_words
process-words:
	@echo "Building dictionary data files..."
	$(MAKE) -C $(PROCESS_WORDS_DIR)
	@echo "Copying dictionary files from process_words..."
	@for file in dictionary.bin.tns index.csv.tns huffman_table.csv.tns; do \
		if [ -f "$(PROCESS_WORDS_DIR)/$$file" ]; then \
			cp "$(PROCESS_WORDS_DIR)/$$file" .; \
			echo "Copied $$file from $(PROCESS_WORDS_DIR)/"; \
		fi \
	done

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.cpp
	$(GXX) $(GCCFLAGS) -c $<
	
%.o: %.S
	$(AS) -c $<

$(EXE).elf: $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $(DISTDIR)/$@ $(LDFLAGS)

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $(DISTDIR)/$^ --output $(DISTDIR)/$@.zehn $(ZEHNFLAGS)
	make-prg $(DISTDIR)/$@.zehn $(DISTDIR)/$@
	rm $(DISTDIR)/$@.zehn

# Copy all files to calcbin folder
copy-to-calcbin: $(EXE).tns
	mkdir -p $(CALCBIN)
	cp $(DISTDIR)/$(EXE).tns $(CALCBIN)/
	@for file in dictionary.bin.tns index.csv.tns huffman_table.csv.tns; do \
		if [ -f "$$file" ]; then \
			cp "$$file" $(CALCBIN)/; \
			echo "Copied $$file to $(CALCBIN)/"; \
		else \
			echo "Warning: $$file not found, skipping"; \
		fi \
	done
	@echo "All files copied to $(CALCBIN)/ folder"

clean:
	rm -f *.o $(DISTDIR)/$(EXE).tns $(DISTDIR)/$(EXE).elf $(DISTDIR)/$(EXE).zehn
	rm -f dictionary.bin.tns index.csv.tns huffman_table.csv.tns
	rm -rf $(CALCBIN)
	$(MAKE) -C $(PROCESS_WORDS_DIR) clean

.PHONY: all clean stats copy-to-calcbin process-words

# Show file sizes and statistics
stats: $(EXE).tns
	@echo "nDictionary file statistics:"
	@echo "============================"
	@if [ -f "$(DISTDIR)/$(EXE).tns" ]; then \
		size=$$(stat -c%s "$(DISTDIR)/$(EXE).tns" 2>/dev/null || stat -f%z "$(DISTDIR)/$(EXE).tns" 2>/dev/null); \
		echo "$(EXE).tns: $$(numfmt --to=iec $$size)"; \
	fi
	@if [ -f "$(DISTDIR)/$(EXE).elf" ]; then \
		size=$$(stat -c%s "$(DISTDIR)/$(EXE).elf" 2>/dev/null || stat -f%z "$(DISTDIR)/$(EXE).elf" 2>/dev/null); \
		echo "$(EXE).elf: $$(numfmt --to=iec $$size)"; \
	fi
	@echo ""
	@echo "Dictionary data files:"
	@echo "====================="
	@for file in dictionary.bin.tns index.csv.tns huffman_table.csv.tns; do \
		if [ -f "$$file" ]; then \
			size=$$(stat -c%s "$$file" 2>/dev/null || stat -f%z "$$file" 2>/dev/null); \
			echo "$$file: $$(numfmt --to=iec $$size)"; \
		else \
			echo "$$file: not found"; \
		fi \
	done
	@if [ -f "index.csv.tns" ]; then \
		echo "Word count: $$(tail -n +2 index.csv.tns | wc -l)"; \
	fi
