all:
	g++ -I src/SDL2/include -L src/SDL2/lib -o build/main src/main.cpp src/CPU.cpp src/Bus.cpp -lmingw32 -lSDL2main -lSDL2