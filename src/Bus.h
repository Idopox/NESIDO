#pragma once

#include <cstdint>
#include <memory>


#include "CPU.h"
#include "PPU.h"
#include "Cartridge.h"

class Bus
{

public:
    Bus();
    ~Bus();

public: 

    CPU cpu;
    PPU ppu;
    
    std::shared_ptr<Cartridge> cart;

    uint8_t cpuRam[2048];


public: 

    void cpuWrite(uint8_t data, uint16_t addr);
    uint8_t cpuRead(uint16_t addr);

public:
    void clock();
    void reset();
    void insertCartridge(std::shared_ptr<Cartridge>& cartridge);

private:
    uint32_t systemClockCounter = 0;
    

    //DMA
    uint8_t dmaPage = 0x00;
    uint8_t dmaOffset = 0x00;
    uint8_t dmaData = 0x00;

    bool isDMA = false;
    bool isDMASync = false;



};
