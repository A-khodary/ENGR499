BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

CC = gcc
CCFLAGS = -g -Wall
CCTAIL = -lbluetooth

all: $(BIN_DIR)/simplescan

$(BIN_DIR)/simplescan: simplescan.c $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) $< -o $@ $(CCTAIL)


.PHONY: clean project
clean:
	rm -rf $(BIN_DIR)

$(BIN_DIR)/.dirstamp:
	mkdir -p $(BIN_DIR)
	touch $(BIN_DIR)/.dirstamp