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

	uint32_t nesPalette[64];


public:
	uint32_t frameBuffer[256 * 240];  // The framebuffer to store the current frame's pixel data
    bool frame_complete = false;

	uint32_t GetColorFromPalette(uint8_t palette, uint8_t pixel);

private:
    int16_t scanline = 0;
	int16_t cycle = 0;


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
			uint8_t nametableX : 1;
			uint8_t nametableY : 1;
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

			uint16_t coarseX : 5;
			uint16_t coarseY : 5;
			uint16_t nametableX : 1;
			uint16_t nametableY : 1;
			uint16_t fineY : 3;
			uint16_t unused : 1;
		};

		uint16_t reg = 0x0000;
	};

	uint8_t fineX = 0x00;

	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_att = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;

	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_att_lo = 0x0000;
	uint16_t bg_shifter_att_hi = 0x0000;


    struct Address {

        loopy_register vram_addr; // Active "pointer" address into nametable to extract background tile info
		loopy_register tram_addr; // Temporary store of information to be "transferred" into "pointer" at various times
        bool addressLatch = false;

        uint8_t getLowAddr() const {
            return (uint8_t)(tram_addr.reg & 0xFF);
        }

        uint8_t getHiAddr() const {
            return (uint8_t)((tram_addr.reg >> 8) & 0xFF);
        }

        void update(uint8_t data) {
            if (addressLatch) {
				tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
				vram_addr = tram_addr;
            } else {
				tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00FF);
            }
            addressLatch = !addressLatch;
        }

		void increment(uint8_t inc) {
			vram_addr.reg += inc;

			if (vram_addr.reg > 0x3FFF) {
				// Mirror down addr above 0x3FFF
				vram_addr.reg &= 0x3FFF;
			}
		}
    } address;
	
	uint8_t ppuDataBuffer = 0x00;	
	

public:
	// Communications with Main Bus
	uint8_t cpuRead(uint16_t addr);
	void    cpuWrite(uint8_t  data, uint16_t addr);

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

	void reset();

	bool nmi = false;


};