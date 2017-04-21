BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

CXX = g++ -std=c++11
CTHREAD = -pthread
FLAGS = -g -Wall
TAILS = -lbluetooth

all: $(BIN_DIR)/main $(BIN_DIR)/main $(BIN_DIR)/.dirstamp

$(BIN_DIR)/main: main.cpp 
	$(CXX) $(FLAGS) $(CTHREAD) $< -o $@ $(TAILS)


.PHONY: clean
clean:
	rm  $(BIN_DIR)/*

$(BIN_DIR)/.dirstamp:
	mkdir -p $(BIN_DIR)
	touch $(BIN_DIR)/.dirstamp
