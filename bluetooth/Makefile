BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

CC = gcc
CCFLAGS = -g -Wall
CCTAIL = -lbluetooth

OBJS = $(BIN_DIR)/scan.o $(BIN_DIR)/rfcomm-send.o $(BIN_DIR)/rfcomm-receive.o

all: $(BIN_DIR)/main

$(BIN_DIR)/scan.o: $(SRC_DIR)/scan.c $(LIB_DIR)/scan.h $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) -c $< -o $@ $(CCTAIL)
	
$(BIN_DIR)/main: main.c $(OBJS) $(LIB_DIR)/config.h $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) $< $(OBJS) -o $@ $(CCTAIL)
	
$(BIN_DIR)/rfcomm-send.o: $(SRC_DIR)/rfcomm-send.c $(LIB_DIR)/rfcomm.h $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) -c $< -o $@ $(CCTAIL)

$(BIN_DIR)/rfcomm-receive.o: $(SRC_DIR)/rfcomm-receive.c $(LIB_DIR)/rfcomm.h $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) -c $< -o $@ $(CCTAIL)
	
.PHONY: clean
clean:
	rm -rf $(BIN_DIR)

$(BIN_DIR)/.dirstamp:
	mkdir -p $(BIN_DIR)
	touch $(BIN_DIR)/.dirstamp