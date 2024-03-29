#include <iostream>
#include "Bus.h"

Bus::Bus()
{
    cpu.ConnectBus(this);    
}

Bus::~Bus()
{
    
}

void Bus::writeController(uint8_t idx, uint8_t value) {
    if (idx < 2) {
        controller[idx] = value;
    }
}

void Bus::cpuWrite(uint8_t data, uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        addr &= 0x7FF;
        cpuRam[addr] = data;
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        addr &= 0x0007;
        ppu.cpuWrite(data, addr);
    }
    else if (addr == 0x4014)
    {
        dmaPage = data;
        dmaOffset = 0x00;
        isDMA = true;
    }
    else if (addr >= 0x4016 && addr <= 0x4017)
    {
        controllerStatus[addr & 0x0001] = controller[addr & 0x0001];
    }
    else if (addr >= 0x4020 && addr <= 0xFFFF)
    {
        cart->cpuWrite(data,addr);
    }
    
    
}

uint8_t Bus::cpuRead(uint16_t addr)
{
    uint8_t data = 0x00;
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        addr &= 0x7FF;
        data = cpuRam[addr];
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        addr &= 0x0007;
        data = ppu.cpuRead(addr);
    }
    else if (addr >= 0x4016 && addr <= 0x4017)
    {
        data = (controllerStatus[addr & 0x0001] & 0x80) > 0;
		controllerStatus[addr & 0x0001] <<= 1;
    }
    else if (addr >= 0x4020 && addr <= 0xFFFF)
    {
        cart->cpuRead(data,addr);
    }
    
    return data;
}

void Bus::reset()
{
    cpu.reset();
    ppu.reset();
    systemClockCounter = 0;

    dmaPage = 0x00;
    dmaOffset = 0x00;
    dmaData = 0x00;
    isDMA = false;
    isDMASync = false;
    
}

void Bus::insertCartridge(std::shared_ptr<Cartridge>& cartridge)
{
    this->cart = cartridge;
    ppu.ConnectCartridge(cartridge);
}

void Bus::clock()
{
    
	ppu.clock();
	// The CPU runs 3 times slower than the PPU so we only call its
	// clock() function every 3 times this function is called. We
	// have a global counter to keep track of this.
	if (systemClockCounter % 3 == 0)
	{
        if (isDMA)
        {
            if (isDMASync)
            {
                if (systemClockCounter % 2 == 0)
                {
                    dmaData = cpuRead((dmaPage << 8) | dmaOffset);
                }
                else
                {
                    ppu.ptrOAM[dmaOffset] = dmaData;
                    dmaOffset++;
                    if (dmaOffset == 0x00)
                    {
                        isDMA = false;
                        isDMASync = false;
                    }
                }
            }
            else
            {
                if (systemClockCounter % 2 == 1)
                {
                    isDMASync = true;
                }
            }
        }
        else
        {
		    cpu.clock();
        }
    }
    if (ppu.nmi)
    {
        ppu.nmi = false;
        cpu.NMI();
    }
    

	systemClockCounter++;
}

