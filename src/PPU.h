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
    uint8_t tblName[2][1024]; // VRAM
    uint8_t tblPattern[2][4096]; // CHR ROM
    uint8_t tblPalette[32]; // Palettes

private:
    int16_t scanline = 0;
	int16_t cycle = 0;

    bool frame_complete = false;

private:
	union 
	{
		struct 
		{
			uint8_t unused : 5;
			uint8_t spriteOverflow : 1;
			uint8_t sprite0hit : 1;
			uint8_t verticalBlank :1;
		};
		uint8_t reg;
		
	}status;

	union 
	{
		struct Bits
		{
			uint8_t tblNameX : 1;
			uint8_t tblNameY : 1;
			uint8_t vramIncrement : 1;
			uint8_t patternSprite : 1;
			uint8_t patternBackground : 1;
			uint8_t spriteSize : 1;
			uint8_t slaveMode : 1;
			uint8_t enableNMI : 1;

			uint8_t vram_addr_increment() const {
        		return vramIncrement ? 32 : 1;
    		}
		}bits;
		uint8_t reg;
		
	}control;

	union 
	{
		struct 
		{
			uint8_t greyscale : 1;
			uint8_t showBackgroundLeft : 1;
			uint8_t showSpritesLeft : 1;
			uint8_t showBackground : 1;
			uint8_t showSprites : 1;
			uint8_t red : 1;
			uint8_t green : 1;
			uint8_t blue : 1;

		};
		uint8_t reg;
		
	}mask;

	union loopy_register
	{
		// Credit to Loopy for working this out :D
		struct
		{

			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};

		uint16_t reg = 0x0000;
	};
	
	
	loopy_register vram_addr; // Active "pointer" address into nametable to extract background tile info
	loopy_register tram_addr; // Temporary store of information to be "transferred" into "pointer" at various times

    struct Address {
        uint16_t fullAddr = 0x0000;
        bool addressLatch = false;

        uint8_t getLowAddr() const {
            return (uint8_t)(fullAddr & 0xFF);
        }

        uint8_t getHiAddr() const {
            return (uint8_t)((fullAddr >> 8) & 0xFF);
        }

        void update(uint8_t data) {
            if (addressLatch) {
				fullAddr = (fullAddr & 0xFF00) | (uint16_t)(data);
            } else {
			    fullAddr = (fullAddr & 0x00FF) | ((uint16_t)(data) << 8);
            }
            addressLatch = !addressLatch;
        }

		void increment(uint8_t inc) {
			uint16_t prevAddr = fullAddr;
			fullAddr += inc;

			if (fullAddr > 0x3FFF) {
				// Mirror down addr above 0x3FFF
				fullAddr &= 0x3FFF;
			}
		}
    } address;
	
	uint8_t ppuDataBuffer = 0x00;	
	

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

	bool nmi = false;


};