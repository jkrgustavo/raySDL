CC = g++

CC_FLAGS = -std=c++20 -Wall -O3 -arch arm64 -Ilib/SDL2/include

LD_FLAGS = -Llib/SDL2/build/.libs -lSDL2-2.0.0

SRC_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(SRC_FILES:%.cpp=%.o)
OBJ_NAME = mainExe
BIN = bin

.PHONY: all clean

all: dirs libs comp

dirs:
	mkdir -p ./$(BIN)/objects

libs:
	cd lib/SDL2 && ./configure; make

comp: $(OBJ_FILES)
	 $(CC) -o $(BIN)/$(OBJ_NAME) $(BIN)/objects/*.o $(LD_FLAGS)

run: all
	 DYLD_LIBRARY_PATH=lib/SDL2/build/.libs:$(DYLD_LIBRARY_PATH) $(BIN)/$(OBJ_NAME)

%.o: %.cpp
	 $(DYLD_LIBRARY_PATH) $(CC) -o $(BIN)/objects/$(@F) -c $< $(CC_FLAGS)

clean:
	rm -rf $(BIN)
