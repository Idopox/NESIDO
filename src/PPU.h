#pragma once
#include <cstdint>
#include <memory>


#include "Cartridge.h"

class PPU
{
public:
    PPU();
    ~PPU();

private:
    uint8_t tblName[2][1024];
    uint8_t tblPattern[2][4096];
    uint8_t tblPalette[32];

private:
    int16_t scanline = 0;
	int16_t cycle = 0;

    bool frame_complete = false;

public:
	// Communications with Main Bus
	uint8_t cpuRead(uint16_t addr);
	void    cpuWrite(uint16_t addr, uint8_t  data);

	// Communications with PPU Bus
	uint8_t ppuRead(uint16_t addr);
	void    ppuWrite(uint16_t addr, uint8_t data);

private:
	// The Cartridge
	std::shared_ptr<Cartridge> cart;

public:
	// Interface
	void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void clock();


};