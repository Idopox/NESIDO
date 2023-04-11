CPP_SOURCES = $(wildcard src/*.cpp)

all: ${CPP_SOURCES}
	g++ -g -I src/SDL2/include -L src/SDL2/lib -o build/main $^ -lmingw32 -lSDL2main -lSDL2