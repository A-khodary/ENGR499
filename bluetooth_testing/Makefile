BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

GCC = gcc
GCCFLAGS =  -o -lbluetooth

all: $(BIN_DIR)/simplescan

$(BIN_DIR)/simplescan: main.cpp $(BIN_DIR)/attackMove.o $(BIN_DIR)/battle.o $(BIN_DIR)/pokemon.o $(BIN_DIR)/.dirstamp
	$(CXX) $(CPPFLAGS) $(BIN_DIR)/attackMove.o $(BIN_DIR)/battle.o $(BIN_DIR)/pokemon.o main.cpp -o $(BIN_DIR)/lab3

$(BIN_DIR)/attackMove.o: $(LIB_DIR)/attackMove.h $(SRC_DIR)/attackMove.cpp $(BIN_DIR)/.dirstamp
	$(CXX) $(CPPFLAGS) -c $(SRC_DIR)/attackMove.cpp -o $(BIN_DIR)/attackMove.o

$(BIN_DIR)/battle.o: $(LIB_DIR)/battle.h $(SRC_DIR)/battle.cpp $(BIN_DIR)/.dirstamp
	$(CXX) $(CPPFLAGS) -c $(SRC_DIR)/battle.cpp -o $(BIN_DIR)/battle.o

$(BIN_DIR)/pokemon.o: $(LIB_DIR)/pokemon.h $(SRC_DIR)/pokemon.cpp $(BIN_DIR)/.dirstamp
	$(CXX) $(CPPFLAGS) -c $(SRC_DIR)/pokemon.cpp -o $(BIN_DIR)/pokemon.o

.PHONY: clean project
clean:
	rm -rf $(BIN_DIR)

$(BIN_DIR)/.dirstamp:
	mkdir -p $(BIN_DIR)
	touch $(BIN_DIR)/.dirstamp

project: $(BIN_DIR)/lab3
	./$(BIN_DIR)/lab3