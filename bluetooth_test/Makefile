BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

CXX = g++ -std=c++11
CTHREAD = -pthread
FLAGS = -g -Wall
TAILS = -lbluetooth

OBJS = $(BIN_DIR)/rfcomm-send.o $(BIN_DIR)/rfcomm-receive.o

all: $(OBJS)

$(BIN_DIR)/rfcomm-send.o: rfcomm-client.cpp $(BIN_DIR)/.dirstamp
	$(CXX) $(FLAGS) -c $< -o $@ $(TAILS)

$(BIN_DIR)/rfcomm-receive.o: rfcomm-server.cpp $(BIN_DIR)/.dirstamp
	$(CXX) $(FLAGS) -c $< -o $@ $(TAILS)
	
.PHONY: clean
clean:
	rm -rf $(BIN_DIR)

$(BIN_DIR)/.dirstamp:
	mkdir -p $(BIN_DIR)
	touch $(BIN_DIR)/.dirstamp