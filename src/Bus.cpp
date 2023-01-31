#include <iostream>
#include "Bus.h"

Bus::Bus()
{
    cpu.ConnectBus(this);

    for (auto &i : cpuRam) i = 0x00;
    
}

Bus::~Bus()
{
    
}

void Bus::cpuWrite(uint8_t data, uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0xFFFF)
    {
        cpuRam[addr] = data;
    }
}

uint8_t Bus::cpuRead(uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0xFFFF)
    {
        uint8_t value = cpuRam[addr];
        return value;
    }
    return 0;
}

