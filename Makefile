CC = g++

CC_FLAGS = -std=c++20 -Wall -O3 -arch arm64 -Ilib/SDL2/include -Ilib/imgui

LD_FLAGS = -Llib/SDL2/build/.libs -lSDL2-2.0.0

SRC_FILES = $(wildcard src/*.cpp) $(wildcard lib/imgui/*.cpp) lib/imgui/backends/imgui_impl_sdl2.cpp lib/imgui/backends/imgui_impl_sdlrenderer2.cpp
OBJ_FILES = $(SRC_FILES:%.cpp=%.o)
OBJ_NAME = mainExe
BIN = bin

.PHONY: all clean

all: dirs libs comp

dirs:
	mkdir -p ./$(BIN)/objects

libs:
	cd lib/SDL2 && make

comp: $(OBJ_FILES)
	$(CC) -o $(BIN)/$(OBJ_NAME) $^ $(LD_FLAGS)

run: all
	 DYLD_LIBRARY_PATH=lib/SDL2/build/.libs:$(DYLD_LIBRARY_PATH) $(BIN)/$(OBJ_NAME)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CC_FLAGS)

clean:
	rm -rf $(BIN) *.o src/*.o lib/imgui/*.o lib/imgui/backends/*.o

wipe: clean
	cd lib/SDL2 && make distclean
	rm -rf .cache

start:
	cd lib/SDL2 && ./configure; make;
	make run
