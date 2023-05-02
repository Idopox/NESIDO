#include <iostream>

#include "PPU.h"

PPU::PPU()
{
    // The NES color palette in RGB format
	uint32_t tempPalette[64] = {
		// Grays
		0x666666, 0x002A88, 0x1412A7, 0x3B00A4, 0x5C007E, 0x6E0040, 0x6C0600, 0x561D00,
		// Greens
		0x333500, 0x0B4800, 0x005200, 0x004F08, 0x00404D, 0x000000, 0x000000, 0x000000,
		// Blues
		0xADADAD, 0x155FD9, 0x4240FF, 0x7527FA, 0xA01ACC, 0xB71E7B, 0xB53120, 0x994E00,
		// Yellows
		0x6B6D00, 0x388700, 0x0C9300, 0x008F32, 0x007C8D, 0x000000, 0x000000, 0x000000,
		// Light Grays
		0xFFFEFF, 0x64B0FF, 0x9290FF, 0xC676FF, 0xF36AFF, 0xFE6ECC, 0xFE8170, 0xEA9E22,
		// Oranges
		0xBCBE00, 0x88D800, 0x5CE430, 0x45E082, 0x48CDDE, 0x4F4F4F, 0x000000, 0x000000,
		// Light Blues
		0xFFFEFF, 0xC0DFFF, 0xD3D2FF, 0xE8C8FF, 0xFBC2FF, 0xFEC4EA, 0xFECCC5, 0xF7D8A5,
		// Light Greens
		0xE4E594, 0xCFEF96, 0xBDF4AB, 0xB3FFCC, 0xB5EBF2, 0xB8B8B8, 0x000000, 0x000000
	};

	// Copy the temporary palette to the class member
    std::copy(std::begin(tempPalette), std::end(tempPalette), std::begin(nesPalette));
}

PPU::~PPU()
{
    
}

uint32_t PPU::GetColorFromPalette(uint8_t palette, uint8_t pixel)
{
	return nesPalette[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
}

uint8_t PPU::cpuRead(uint16_t addr)
{
    uint8_t data = 0x00;

	switch (addr)
	{
	case 0x0000: // Control
		break;
	case 0x0001: // Mask
		break;
	case 0x0002: // Status
		data = (status.reg & 0xE0) | (ppuDataBuffer & 0x1F);
		status.verticalBlank = 0;
		address.addressLatch = false;
		break;
	case 0x0003: // OAM Address
		break;
	case 0x0004: // OAM Data
		break;
	case 0x0005: // Scroll
		break;
	case 0x0006: // PPU Address - not readable
		break;
	case 0x0007: // PPU Data
		data = ppuDataBuffer;
		ppuDataBuffer = ppuRead(address.vram_addr.reg);

		if (address.vram_addr.reg > 0x3F00) data = ppuDataBuffer;

		address.increment(control.bits.vram_addr_increment());
		break;
	}

	return data;
}

void PPU::cpuWrite( uint8_t  data, uint16_t addr)
{
    switch (addr)
	{
		case 0x0000: // Control
			control.reg = data;
			address.tram_addr.nametableX = control.bits.nametableX;
			address.tram_addr.nametableY = control.bits.nametableY;
			break;
		case 0x0001: // Mask
			mask.reg = data;
			break;
		case 0x0002: // Status
			break;
		case 0x0003: // OAM Address
			break;
		case 0x0004: // OAM Data
			break;
		case 0x0005: // Scroll
			if (address.addressLatch)
			{
				address.tram_addr.fineY = data & 0x07;
				address.tram_addr.coarseY = data >> 3;
			}
			else
			{
				fineX = data & 0x07;
				address.tram_addr.coarseX = data >> 3;
			}
			address.addressLatch = !address.addressLatch;
			break;
		case 0x0006: // PPU Address
			address.update(data);
			break;
		case 0x0007: // PPU Data
			ppuWrite(address.vram_addr.reg, data);

			address.increment(control.bits.vram_addr_increment());
			break;
	}
}

uint8_t PPU::ppuRead(uint16_t addr)
{
    uint8_t data = 0x00;
	addr &= 0x3FFF;


    if (cart->ppuRead(data,addr))
	{

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		data = tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF];
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{
		addr &= 0x0FFF;

		if (cart->mirror == Cartridge::MIRRORING::VERTICAL)
		{
			// Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = tblName[1][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = tblName[1][addr & 0x03FF];
		}
		else if (cart->mirror == Cartridge::MIRRORING::HORIZONTAL)
		{
			// Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = tblName[1][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = tblName[1][addr & 0x03FF];
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
	{
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		data = tblPalette[addr] & (mask.greyscale ? 0x30 : 0x3F);
	}	

	return data;
}

void PPU::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;

	if (cart->ppuWrite(data,addr))
	{

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{
		addr &= 0x0FFF;
		if (cart->mirror == Cartridge::MIRRORING::VERTICAL)
		{
			// Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				tblName[1][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				tblName[1][addr & 0x03FF] = data;
		}
		else if (cart->mirror == Cartridge::MIRRORING::HORIZONTAL)
		{
			// Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				tblName[1][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				tblName[1][addr & 0x03FF] = data;
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
	{
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		tblPalette[addr] = data;
	}


}

void PPU::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    this->cart = cartridge;
}

void PPU::reset()
{
	fineX = 0x00;
	address.addressLatch = false;
	ppuDataBuffer = 0x00;
	scanline = 0;
	cycle = 0;
	bg_next_tile_id = 0x00;
	bg_next_tile_att = 0x00;
	bg_next_tile_lsb = 0x00;
	bg_next_tile_msb = 0x00;
	bg_shifter_pattern_lo = 0x0000;
	bg_shifter_pattern_hi = 0x0000;
	bg_shifter_att_lo = 0x0000;
	bg_shifter_att_hi = 0x0000;
	status.reg = 0x00;
	mask.reg = 0x00;
	control.reg = 0x00;
	address.vram_addr.reg = 0x0000;
	address.tram_addr.reg = 0x0000;
}

void PPU::clock()
{
	//the clock function based on the https://www.nesdev.org/wiki/PPU_frame_timing explanation
	// this diagram visualize it: https://www.nesdev.org/w/images/default/4/4f/Ppu.svg


	// The lambda functions (functions inside functions) contain the various
	// actions to be performed depending upon the output of the state machine
	// for a given scanline/cycle combination

	auto IncrementScrollX = [&]()
	{
		if (mask.showBackground || mask.showSprites)
		{
			//check if passed current nametable
			if (address.vram_addr.coarseX == 31)
			{
				address.vram_addr.coarseX = 0;
				address.vram_addr.nametableX = ~address.vram_addr.nametableX;
			}
			else
			{
				address.vram_addr.coarseX++;
			}
			
		}
	};

	auto IncrementScrollY = [&]()
	{
		if (mask.showBackground || mask.showSprites)
		{
			if (address.vram_addr.fineY < 7)
			{
				address.vram_addr.fineY++;
			}
			else
			{
				address.vram_addr.fineY = 0;
				if (address.vram_addr.coarseY == 29)
				{
					address.vram_addr.coarseY = 0;
					address.vram_addr.nametableY = ~address.vram_addr.nametableY;
				}
				else if (address.vram_addr.coarseY == 31)
				{
					address.vram_addr.coarseY = 0;
				}
				else
				{
					address.vram_addr.coarseY++;
				}
			}
		}
	};
	
	auto TransferAddressX = [&]()
	{
		if (mask.showBackground || mask.showSprites)
		{
			address.vram_addr.nametableX = address.tram_addr.nametableX;
			address.vram_addr.coarseX = address.tram_addr.coarseX;
		}
	};

	auto TransferAddressY = [&]()
	{
		if (mask.showBackground || mask.showSprites)
		{
			address.vram_addr.fineY = address.tram_addr.fineY;
			address.vram_addr.nametableY = address.tram_addr.nametableY;
			address.vram_addr.coarseY = address.tram_addr.coarseY;
		}
	};

	auto LoadBackgroundShifters = [&]()
	{
		bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
		bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;
		bg_shifter_att_lo = (bg_shifter_att_lo & 0xFF00) | ((bg_next_tile_att & 0b01) ? 0xFF : 0x00);
		bg_shifter_att_hi = (bg_shifter_att_hi & 0xFF00) | ((bg_next_tile_att & 0b10) ? 0xFF : 0x00);
	};

	auto UpdateShifters = [&]()
	{
		if (mask.showBackground)
		{
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<=1;
		}
	};


	if (scanline >= -1 && scanline < 240)
	{

		if (scanline == 0 && cycle == 0)
		{
			// "Odd Frame" cycle skip
			cycle = 1;
		}

		// top left of the screen means we finished the vblank phase
		if (scanline == -1 && cycle == 1) 
		{
			status.verticalBlank = 1;
		}

		// visible cycles
		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338))
		{
			UpdateShifters();
			switch ((cycle-1) % 8 )
			{
				case 0:
					LoadBackgroundShifters();
					// Fetch the next background tile ID
					// "(vram_addr.reg & 0x0FFF)" : Mask to 12 bits that are relevant
					// "| 0x2000"                 : Offset into nametable space on PPU address bus
					bg_next_tile_id = ppuRead(0x2000 | (address.vram_addr.reg & 0x0FFF));
					break;
				case 2:
					// Fetch the next background tile attribute.
					bg_next_tile_att = ppuRead(0x23C0 | (address.vram_addr.nametableY << 11) | (address.vram_addr.nametableX << 10)
					| ((address.vram_addr.coarseY >> 2) << 3)
					| (address.vram_addr.coarseX >> 2));

					if (address.vram_addr.coarseY & 0x02) bg_next_tile_att >>= 4;
					if (address.vram_addr.coarseX & 0x02) bg_next_tile_att >>= 2;
					bg_next_tile_att &= 0x03;
					break;
				case 4:
					bg_next_tile_lsb = ppuRead((control.bits.patternBackground << 12) + ((uint16_t)bg_next_tile_id << 4) + (address.vram_addr.fineY) + 0);
					break;
				case 6:
					bg_next_tile_msb = ppuRead((control.bits.patternBackground << 12) + ((uint16_t)bg_next_tile_id << 4) + (address.vram_addr.fineY) + 8);
					break;
				case 7:
					IncrementScrollX();
					break;
			
			}
		}
		
		if (cycle == 256)
		{
			IncrementScrollY();
		}

		if (cycle == 257)
		{
			TransferAddressX();
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305)
		{
			TransferAddressY();
		}
	}
	

	if (scanline == 240)
	{
		//Nothing happens
	}

	//starting vblank phase
	if (scanline >= 241 && scanline < 261)
	{
		if (scanline == 241 && cycle == 1)
		{
			status.verticalBlank = 1;
			if (control.bits.enableNMI)
			{
				nmi = true;
			}
			
		}
	}

	uint8_t bgPixel = 0x00;
	uint8_t bgPalette = 0x00;
	
	if (mask.showBackground)
	{
		uint16_t bitMux = 0x8000 >> fineX;

		uint8_t p0Pixel = (bg_shifter_pattern_lo & bitMux) > 0;
		uint8_t p1Pixel = (bg_shifter_pattern_hi & bitMux) > 0;
		bgPixel = (p1Pixel << 1 ) | p0Pixel;

		uint8_t bgPal0 = (bg_shifter_att_lo & bitMux) > 0;
		uint8_t bgPal1 = (bg_shifter_att_lo & bitMux) > 0;
		bgPalette = (bgPal1 << 1 ) | bgPal0;
	}

	//this line will return the actual RGB color pixel that should be drawn
	uint32_t pixelColor = GetColorFromPalette(bgPalette, bgPixel);

	// Set the pixel in the framebuffer
	//frameBuffer[256 * scanline + cycle - 1] = pixelColor;
	if (scanline >= 0 && scanline < 240 && cycle > 0 && cycle <= 256)
	{
		frameBuffer[256 * scanline + cycle - 1] = pixelColor;
	}
    cycle++;
	//std::cout << "cycles :" << cycle << " scanlies: " << scanline << std::endl;
	if (cycle >= 341)
	{
		cycle = 0;
		scanline++;
		
		if (scanline >= 261)
		{
			scanline = -1;
			frame_complete = true;
		}
	}
}

