#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <windows.h>
#define main SDL_main
#include <SDL2/SDL.h>
#include "CPU.h"
#include "Bus.h"


const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

const uint8_t BUTTONA = 0x80;
const uint8_t BUTTTONB = 0x40;
const uint8_t SELECT = 0x20;
const uint8_t START = 0x10;
const uint8_t UP = 0x08;
const uint8_t DOWN = 0x04;
const uint8_t LEFT = 0x02;
const uint8_t RIGHT = 0x01;

bool initSDL() {

    putenv("SDL_VIDEODRIVER=windows");
    if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
        std::cout << "SDL Init Failed." << std::endl;
        return false;
    }
    std::cout << "SDL Init succeeded." << std::endl;
    return true;
}

SDL_Window* createWindow() {
    SDL_Window* window = SDL_CreateWindow("NESIDO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256 * 2, 240 * 2, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return nullptr;
    }
    std::cout << "SDL window succeeded." << std::endl;
    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return nullptr;
    }
    SDL_RenderSetLogicalSize(renderer, 256, 240);
    return renderer;
}

SDL_Texture* createFrameTexture(SDL_Renderer* renderer)
{
    // Create the texture for the frame
    SDL_Texture* frameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    if (frameTexture == nullptr) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
    }
    return frameTexture;
}

void cleanupSDL(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* frameTexture) {
    SDL_DestroyTexture(frameTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void logDebug(uint16_t addr, uint8_t a, uint8_t x, uint8_t y, uint8_t p, uint8_t sp) {
  static std::ofstream logFile;
  if (!logFile.is_open()) {
    logFile.open("debug.log", std::ios::out | std::ios::trunc);
  }

  logFile << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << addr << " ";
  logFile << "A:" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)a << " ";
  logFile << "X:" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)x << " ";
  logFile << "Y:" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)y << " ";
  logFile << "P:" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)p << " ";
  logFile << "SP:" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)sp << "\n";
}

Bus nes;
std::shared_ptr<Cartridge> cart;

const int WIDTH = 256, HEIGHT = 240;
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Invalid arguments amount" << std::endl;
        return 1;
    }

    std::string game_name(argv[1]);
    std::filesystem::path roms_directory("../roms");
    std::filesystem::path game_path = roms_directory / game_name;

    if (!(std::filesystem::exists(game_path))) {
        std::filesystem::path game_path = game_name;
        if (!(std::filesystem::exists(game_path))) {
            std::cout << "The game \"" << game_name << "\" does not exist in the roms directory." << std::endl;
            return 1;
        }
    }
    
    if (!initSDL()) return 1;

    SDL_Window* window = createWindow();
    if (window == nullptr) return 1;

    SDL_Renderer* renderer = createRenderer(window);
    if (renderer == nullptr) return 1;

    SDL_RenderSetLogicalSize(renderer, 256, 240);

    SDL_Texture* frameTexture = createFrameTexture(renderer);
    if (frameTexture == nullptr) return 1;
    std::cout << game_path.generic_string() << std::endl;
    cart = std::make_shared<Cartridge>(game_path.generic_string());

    nes.insertCartridge(cart);

    nes.reset();

    SDL_Event event;
    bool isRunning = true;
    while (isRunning)
    {
        uint64_t startCounter = SDL_GetPerformanceCounter();

        while(SDL_PollEvent(&event))
        {
            
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
                break;
            }

            if (event.type == SDL_KEYDOWN)
            {                
                switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            isRunning = false;
                            break;
                    }
            }
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);

        uint8_t controllerState = 0;
        controllerState |= keyState[SDL_SCANCODE_X] ? BUTTONA : 0x00; // A Button
        controllerState |= keyState[SDL_SCANCODE_Z] ? BUTTTONB : 0x00; // B Button
        controllerState |= keyState[SDL_SCANCODE_A] ? SELECT : 0x00; // Select
        controllerState |= keyState[SDL_SCANCODE_S] ? START : 0x00; // Start
        controllerState |= keyState[SDL_SCANCODE_UP] ? UP : 0x00;
        controllerState |= keyState[SDL_SCANCODE_DOWN] ? DOWN : 0x00;
        controllerState |= keyState[SDL_SCANCODE_LEFT] ? LEFT : 0x00;
        controllerState |= keyState[SDL_SCANCODE_RIGHT] ? RIGHT : 0x00;

        nes.writeController(0, controllerState);
        nes.writeController(1, controllerState);

        do 
        {
            nes.clock();
        }while (!nes.ppu.frame_complete);
        // do 
        // {
        //     nes.cpu.clock();
        // } while (!nes.cpu.complete());
        // logDebug(nes.cpu.pc,nes.cpu.a,nes.cpu.x, nes.cpu.y, nes.cpu.status,nes.cpu.sp);

        // Update the texture with the framebuffer data
        SDL_UpdateTexture(frameTexture, nullptr, nes.ppu.frameBuffer, 256 * sizeof(uint32_t));
        
        // Render the texture to the window
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, frameTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // Reset the frame_complete flag
        nes.ppu.frame_complete = false;


        uint64_t endCounter = SDL_GetPerformanceCounter();
        uint64_t counterDiff = endCounter - startCounter;
        double frameTime = (double)counterDiff / SDL_GetPerformanceFrequency() * 1000.0;
        if (frameTime < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - (uint32_t)frameTime);
        }
    }

    cleanupSDL(window, renderer, frameTexture);

    return 0;
}



// std::string hex(uint32_t n, uint8_t d)
// {
//     std::string s(d, '0');
//     for (int i = d - 1; i >= 0; i--, n >>= 4)
//         s[i] = "0123456789ABCDEF"[n & 0xF];
//     return s;
// }

// void DrawCpu()
// {
//     std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
//     std::string status = "STATUS: ";
//     std::cout <<"STATUS:" << std::endl;
//     std::cout <<"N: ";
//     nes.cpu.status & CPU::N ? std::cout << 1 << "   " : std::cout << 0 << "   ";
//     std::cout <<"V: ";
//     nes.cpu.status & CPU::V ? std::cout << 1 << "   " : std::cout << 0 << "   ";
//     std::cout <<"U: ";
//     nes.cpu.status & CPU::U ? std::cout << 1 << "   " : std::cout << 0 << "   ";
//     std::cout <<"B: ";
//     nes.cpu.status & CPU::B ? std::cout << 1 << "   " : std::cout << 0 << "   ";
//     std::cout <<"D: ";
//     nes.cpu.status & CPU::D ? std::cout << 1 << "   " : std::cout << 0 << "   ";
//     std::cout <<"I: ";
//     nes.cpu.status & CPU::I ? std::cout << 1 << "   " : std::cout << 0 << "   ";
//     std::cout <<"Z: ";
//     nes.cpu.status & CPU::Z ? std::cout << 1 << "   " : std::cout << 0 << "   ";
//     std::cout <<"C: ";
//     nes.cpu.status & CPU::C ? std::cout << 1 << std::endl: std::cout << 0 << std::endl;
//     std::cout << "PC: $" << hex(nes.cpu.pc, 4) << std::endl;
//     std::cout << "A: $" << hex(nes.cpu.a, 2)<< std::endl;
//     std::cout << "X: $" << hex(nes.cpu.x, 2) << std::endl;
//     std::cout << "Y: $" << hex(nes.cpu.y, 2) << std::endl;
//     std::cout << "SP: $" << hex(nes.cpu.sp, 2) << std::endl;
//     std::cout << std::endl;
//     std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
// }


