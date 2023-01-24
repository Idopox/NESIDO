
#include "Bus.h"

Bus::Bus()
{
    cpu.ConnectBus(this);
}

Bus::~Bus()
{
    
}

void Bus::cpuWrite(uint8_t data, uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0x2000)
    {
        cpuRam[addr] = data;
    }
}

uint8_t Bus::cpuRead(uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0x2000)
    {
        return cpuRam[addr];
    }
}

