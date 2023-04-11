#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <windows.h>
#define main SDL_main
#include <SDL2/SDL.h>
#include "CPU.h"
#include "Bus.h"


const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;


Bus nes;
std::shared_ptr<Cartridge> cart;



std::string hex(uint32_t n, uint8_t d)
{
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
};

void DrawCpu()
{
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
    std::string status = "STATUS: ";
    std::cout <<"STATUS:" << std::endl;
    std::cout <<"N: ";
    nes.cpu.status & CPU::N ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"V: ";
    nes.cpu.status & CPU::V ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"U: ";
    nes.cpu.status & CPU::U ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"B: ";
    nes.cpu.status & CPU::B ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"D: ";
    nes.cpu.status & CPU::D ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"I: ";
    nes.cpu.status & CPU::I ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"Z: ";
    nes.cpu.status & CPU::Z ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"C: ";
    nes.cpu.status & CPU::C ? std::cout << 1 << std::endl: std::cout << 0 << std::endl;
    std::cout << "PC: $" << hex(nes.cpu.pc, 4) << std::endl;
    std::cout << "A: $" << hex(nes.cpu.a, 2)<< std::endl;
    std::cout << "X: $" << hex(nes.cpu.x, 2) << std::endl;
    std::cout << "Y: $" << hex(nes.cpu.y, 2) << std::endl;
    std::cout << "SP: $" << hex(nes.cpu.sp, 2) << std::endl;
    std::cout << std::endl;
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
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

const int WIDTH = 320, HEIGHT = 320;
int main(int argc, char** argv)
{
    putenv("SDL_VIDEODRIVER=windows");
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)    
    {
       std::cout<<"Endl Init Failed."<<std::endl;
       return 1;    
    }
   std::cout<<"SDL Init succeeded."<<std::endl;

    // Check that the window was successfully created
    SDL_Window *window = SDL_CreateWindow("aaaaa", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,0);
    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }
       std::cout<<"SDL window succeeded."<<std::endl;
    

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    cart = std::make_shared<Cartridge>("../nestest.nes");

    nes.insertCartridge(cart);

    // Reset
     nes.cpu.reset();
    SDL_Event event;
    bool isRunning = true;
    while (isRunning)
    {
        uint32_t startTicks = SDL_GetTicks();

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
                        case SDLK_SPACE:
                            do
                            {
                                nes.cpu.clock();
                            } while (!nes.cpu.complete());
                            DrawCpu();
                            break;
                    }
            }
                    
            
        }

        if (nes.cpu.pc == 0xC825)
        {
            asm("nop");
        }
        do
        {
            nes.cpu.clock();

            
			//do { nes.clock(); } while (!nes.ppu.frame_complete);
			//nes.ppu.frame_complete = false;
        } while (!nes.cpu.complete());

        logDebug(nes.cpu.pc,nes.cpu.a,nes.cpu.x, nes.cpu.y, nes.cpu.status,nes.cpu.sp);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // uint32_t frameTicks = SDL_GetTicks() - startTicks;
        // if (frameTicks < SCREEN_TICKS_PER_FRAME) {
        //     SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        // }
        if (nes.cpu.pc == 0xC66E)
        {
            break;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    


    return 0;
}
