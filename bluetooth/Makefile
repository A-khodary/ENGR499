BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

CC = gcc
CCFLAGS = -g -Wall
CCTAIL = -lbluetooth

all: $(BIN_DIR)/main

$(BIN_DIR)/scan.o: $(SRC_DIR)/scan.c $(LIB_DIR)/scan.h $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) -c $< -o $@ $(CCTAIL)
	
$(BIN_DIR)/main: main.c $(BIN_DIR)/scan.o $(LIB_DIR)/config.h $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) $< $(BIN_DIR)/scan.o -o $@ $(CCTAIL)


.PHONY: clean
clean:
	rm -rf $(BIN_DIR)

$(BIN_DIR)/.dirstamp:
	mkdir -p $(BIN_DIR)
	touch $(BIN_DIR)/.dirstamp