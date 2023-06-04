CPP_SOURCES = $(wildcard src/*.cpp)
OBJ = $(addprefix build/obj/, $(notdir ${CPP_SOURCES:.cpp=.o}))

all: build/nesido.exe

build/nesido.exe: $(OBJ)
	g++ -std=c++17 -I src/SDL2/include -L src/SDL2/lib -o build/nesido $^ -lmingw32 -lSDL2main -lSDL2

build/obj/%.o: src/%.cpp
	@if not exist build\obj mkdir build\obj
	g++ -std=c++17 -I src/SDL2/include -L src/SDL2/lib -c -Wall -Wno-unused-value -Wno-unused-variable -g -o $@ $< -lmingw32 -lSDL2main -lSDL2