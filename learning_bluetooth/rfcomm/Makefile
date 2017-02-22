BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

CC = gcc
CCFLAGS = -g -Wall
CCTAIL = -lbluetooth

all: $(BIN_DIR)/rfcomm-send $(BIN_DIR)/rfcomm-receive

$(BIN_DIR)/rfcomm-send: rfcomm-client.c $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) $< -o $@ $(CCTAIL)
	
$(BIN_DIR)/rfcomm-receive: rfcomm-server.c $(BIN_DIR)/.dirstamp
	$(CC) $(CCFLAGS) $< -o $@ $(CCTAIL)


.PHONY: clean send receive
clean:
	rm -rf $(BIN_DIR)
	
send: $(BIN_DIR)/rfcomm-send

receive: $(BIN_DIR)/rfcomm-receive

$(BIN_DIR)/.dirstamp:
	mkdir -p $(BIN_DIR)
	touch $(BIN_DIR)/.dirstamp