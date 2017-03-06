BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

CXX = g++ -std=c++11
CTHREAD = -pthread
FLAGS = -g -Wall
TAILS = -lbluetooth

OBJS = $(BIN_DIR)/scan.o $(BIN_DIR)/rfcomm-send.o $(BIN_DIR)/rfcomm-receive.o

all: $(BIN_DIR)/main

$(BIN_DIR)/main: main.cpp $(OBJS) $(LIB_DIR)/prompt.h $(LIB_DIR)/config.h $(BIN_DIR)/.dirstamp
	$(CXX) $(FLAGS) $(CTHREAD) $< $(OBJS) -o $@ $(TAILS)
	
$(BIN_DIR)/scan.o: $(SRC_DIR)/scan.cpp $(LIB_DIR)/scan.h $(BIN_DIR)/.dirstamp
	$(CXX) $(FLAGS) -c $< -o $@ $(TAILS)
	
$(BIN_DIR)/rfcomm-send.o: $(SRC_DIR)/rfcomm-send.cpp $(LIB_DIR)/rfcomm.h $(BIN_DIR)/.dirstamp
	$(CXX) $(FLAGS) -c $< -o $@ $(TAILS)

$(BIN_DIR)/rfcomm-receive.o: $(SRC_DIR)/rfcomm-receive.cpp $(LIB_DIR)/rfcomm.h $(BIN_DIR)/.dirstamp
	$(CXX) $(FLAGS) -c $< -o $@ $(TAILS)
	
.PHONY: clean
clean:
	rm -rf $(BIN_DIR)

$(BIN_DIR)/.dirstamp:
	mkdir -p $(BIN_DIR)
	touch $(BIN_DIR)/.dirstamp