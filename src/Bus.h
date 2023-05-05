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


private:


   
public:
    void clock();
    void reset();
    void insertCartridge(std::shared_ptr<Cartridge>& cartridge);

    void writeController(uint8_t idx, uint8_t value);

private:
    uint32_t systemClockCounter = 0;

    //joypads
    uint8_t controller[2];
    uint8_t controllerStatus[2];

    //DMA
    uint8_t dmaPage = 0x00;
    uint8_t dmaOffset = 0x00;
    uint8_t dmaData = 0x00;

    bool isDMA = false;
    bool isDMASync = false;



};
