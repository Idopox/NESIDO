CPP_SOURCES = $(wildcard Emulator/src/*.cpp)
OBJ = $(addprefix Emulator/build/obj/, $(notdir ${CPP_SOURCES:.cpp=.o}))

all: Emulator/build/nesido.exe

Emulator/build/nesido.exe: $(OBJ)
	g++ -std=c++17 -I Emulator/src/SDL2/include -L Emulator/src/SDL2/lib -o Emulator/build/nesido $^ -lmingw32 -lSDL2main -lSDL2

Emulator/build/obj/%.o: Emulator/src/%.cpp
	@if not exist Emulator\build\obj mkdir Emulator\build\obj
	g++ -std=c++17 -I Emulator/src/SDL2/include -L Emulator/src/SDL2/lib -c -Wall -Wno-unused-value -Wno-unused-variable -g -o $@ $< -lmingw32 -lSDL2main -lSDL2