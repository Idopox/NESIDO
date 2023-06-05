#include <iostream>
#include <cstring>
#include "PPU.h"

PPU::PPU()
{
    // The NES color palette in RGB format
	uint32_t tempPalette[64] = {
		// Grays
		0x666666FF, 0x002A88FF, 0x1412A7FF, 0x3B00A4FF, 0x5C007EFF, 0x6E0040FF, 0x6C0600FF, 0x561D00FF,
		// Greens
		0x333500FF, 0x0B4800FF, 0x005200FF, 0x004F08FF, 0x00404DFF, 0x000000FF, 0x000000FF, 0x000000FF,
		// Blues
		0xADADADFF, 0x155FD9FF, 0x4240FFFF, 0x7527FAFF, 0xA01ACCFF, 0xB71E7BFF, 0xB53120FF, 0x994E00FF,
		// Yellows
		0x6B6D00FF, 0x388700FF, 0x0C9300FF, 0x008F32FF, 0x007C8DFF, 0x000000FF, 0x000000FF, 0x000000FF,
		// Light Grays
		0xFFFEFFFF, 0x64B0FFFF, 0x9290FFFF, 0xC676FFFF, 0xF36AFFFF, 0xFE6ECCFF, 0xFE8170FF, 0xEA9E22FF,
		// Oranges
		0xBCBE00FF, 0x88D800FF, 0x5CE430FF, 0x45E082FF, 0x48CDDEFF, 0x4F4F4FFF, 0x000000FF, 0x000000FF,
		// Light Blues
		0xFFFEFFFF, 0xC0DFFFFF, 0xD3D2FFFF, 0xE8C8FFFF, 0xFBC2FFFF, 0xFEC4EAFF, 0xFECCC5FF, 0xF7D8A5FF,
		// Light Greens
		0xE4E594FF, 0xCFEF96FF, 0xBDF4ABFF, 0xB3FFCCFF, 0xB5EBF2FF, 0xB8B8B8FF, 0x000000FF, 0x000000FF
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
	case 0x0003: // OAM Address - not readable
		break;
	case 0x0004: // OAM Data
		data = ptrOAM[oamAddr];
		break;
	case 0x0005: // Scroll
		break;
	case 0x0006: // PPU Address - not readable
		break;
	case 0x0007: // PPU Data
		data = ppuDataBuffer;
		ppuDataBuffer = ppuRead(address.vram_addr.reg);

		if (address.vram_addr.reg >= 0x3F00) data = ppuDataBuffer;

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
			oamAddr = data;
			break;
		case 0x0004: // OAM Data
			ptrOAM[oamAddr] = data;
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
		
		bg_shifter_att_lo  = (bg_shifter_att_lo & 0xFF00) | ((bg_next_tile_att & 0b01) ? 0xFF : 0x00);
		bg_shifter_att_hi  = (bg_shifter_att_hi & 0xFF00) | ((bg_next_tile_att & 0b10) ? 0xFF : 0x00);
	};

	auto UpdateShifters = [&]()
	{
		if (mask.showBackground)
		{
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<=1;

			bg_shifter_att_lo <<= 1;
			bg_shifter_att_hi <<= 1;
		}
		
		if (mask.showSprites && cycle >= 1 && cycle < 258)
		{
			for (uint8_t i = 0; i < spritesCount; i++)
			{
				if (spritesCurrentScanline[i].x > 0)
				{
					spritesCurrentScanline[i].x--;
				}
				else
				{
					sprite_shifter_pattern_lo[i] <<= 1;
					sprite_shifter_pattern_hi[i] <<= 1;
				}
			}
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
			status.verticalBlank = 0;

			status.spriteOverflow = 0;

			status.sprite0hit = 0;
			
			for (uint8_t i = 0; i < 8; i++)
			{
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}
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
					bg_next_tile_att = ppuRead(0x23C0 | (address.vram_addr.nametableY << 11) 
					                                 | (address.vram_addr.nametableX << 10) 
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
			LoadBackgroundShifters();
			TransferAddressX();
		}

		// Superfluous reads of tile id at end of scanline
		if (cycle == 338 || cycle == 340)
		{
			bg_next_tile_id = ppuRead(0x2000 | (address.vram_addr.reg & 0x0FFF));
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305)
		{
			TransferAddressY();
		}



		if (cycle == 257 && scanline >= 0)
		{
			std::memset(spritesCurrentScanline, 0xFF, 8* sizeof(objectAtt));
			spritesCount = 0;
			for (uint8_t i = 0; i < 8; i++)
			{
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}
			
			uint8_t OAMEntry = 0;

			sprite0hit = false;

			while (OAMEntry < 64 && spritesCount < 9)
			{
				int16_t difference = ((uint16_t)scanline - (uint16_t)OAM[OAMEntry].y);

				if (difference >= 0 && difference < (control.bits.spriteSize ? 16: 8))
				{
					if (spritesCount < 8)
					{
						if (OAMEntry == 0)
						{
							sprite0hit = true;
						}
						
						memcpy(&spritesCurrentScanline[spritesCount], &OAM[OAMEntry], sizeof(objectAtt));
					}
					spritesCount++;
				}
				OAMEntry++;
			}
			status.spriteOverflow = (spritesCount>8);
		}

		if (cycle == 340)
		{
			for (uint8_t i = 0; i < spritesCount; i++)
			{
				uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
				uint16_t  sprite_pattern_addr_lo, sprite_pattern_addr_hi;

				if (!control.bits.spriteSize)
				{
					//8x8
					if (!(spritesCurrentScanline[i].att & 0x80))
					{
						//not flipped vertically

						sprite_pattern_addr_lo = (control.bits.patternSprite << 12) 
							| (spritesCurrentScanline[i].id << 4)
							| (scanline- spritesCurrentScanline[i].y);

					}
					else
					{
						//flipped vertically
						sprite_pattern_addr_lo = (control.bits.patternSprite << 12) 
							| (spritesCurrentScanline[i].id << 4)
							| (7 - (scanline- spritesCurrentScanline[i].y));
					}
				}
				else
				{
					//8x16
					if (!(spritesCurrentScanline[i].att & 0x80))
					{
						//not flipped vertically
						if (scanline - spritesCurrentScanline[i].y < 8)
						{
							// top half
							sprite_pattern_addr_lo = ((spritesCurrentScanline[i].id & 0x01) << 12) 
								| ((spritesCurrentScanline[i].id & 0xFE) << 4)
								| ((scanline- spritesCurrentScanline[i].y) & 0x07);
						}
						else
						{
							//bottom half
							sprite_pattern_addr_lo = ((spritesCurrentScanline[i].id & 0x01) << 12) 
								| (((spritesCurrentScanline[i].id & 0xFE)+1) << 4)
								| ((scanline- spritesCurrentScanline[i].y) & 0x07);
							
						}
						

					}
					else
					{
						//flipped vertically
						if (scanline - spritesCurrentScanline[i].y < 8)
						{
							// top half
							sprite_pattern_addr_lo = ((spritesCurrentScanline[i].id & 0x01) << 12) 
								| (((spritesCurrentScanline[i].id & 0xFE)+1) << 4)
								| ((7- (scanline- spritesCurrentScanline[i].y)) & 0x07);
						}
						else
						{
							sprite_pattern_addr_lo = ((spritesCurrentScanline[i].id & 0x01) << 12) 
								| (((spritesCurrentScanline[i].id & 0xFE)) << 4)
								| ((7- (scanline- spritesCurrentScanline[i].y)) & 0x07); //TODO
						}
					}
				}

				sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

				sprite_pattern_bits_lo = ppuRead(sprite_pattern_addr_lo);
				sprite_pattern_bits_hi = ppuRead(sprite_pattern_addr_hi);

				if (spritesCurrentScanline[i].att & 0x40)
				{
					//flipped horizontaly

					// https://stackoverflow.com/a/2602885
					auto flipbyte = [](uint8_t b)
					{
						b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
						b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
						b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
						return b;
					};

					sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
					sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
				}	
				
				sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
				sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
				
			}

			
			
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
		uint8_t bgPal1 = (bg_shifter_att_hi & bitMux) > 0;
		bgPalette = (bgPal1 << 1 ) | bgPal0;
	}

	uint8_t fgPixel = 0x00;
	uint8_t fgPalette = 0x00;
	uint8_t fgPriority = 0x00;
	if (mask.showSprites)
	{
		sprite0rendered = false;

		for (uint8_t i = 0; i < spritesCount; i++)
		{
			if (spritesCurrentScanline[i].x == 0)
			{
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
				fgPixel = (fg_pixel_hi << 1) | fg_pixel_lo;

				fgPalette = (spritesCurrentScanline[i].att & 0x03) + 0x04;
				fgPriority = (spritesCurrentScanline[i].att & 0x20) == 0;

				if (fgPixel != 0)
				{
					if (i == 0)
					{
						sprite0rendered = true;
					}
					break;
				}
			}
		}
	}

	uint8_t pixel = 0x00; // the final pixel to be drawn (fg / bg)
	uint8_t palette = 0x00; // the final palette

	if (fgPixel == 0 && bgPixel == 0)
	{
		//both transparent
		uint8_t pixel = 0x00;
		uint8_t palette = 0x00;
	}
	else if (bgPixel == 0 && fgPixel > 0)
	{
		// bg transparent, fg visible
		pixel = fgPixel;
		palette = fgPalette;
	}
	else if (bgPixel > 0 && fgPixel == 0)
	{
		// bg visible, fg transparent
		pixel = bgPixel;
		palette = bgPalette;
	}
	else
	{
		if (fgPriority)
		{
			pixel = fgPixel;
			palette = fgPalette;
		}
		else
		{
			pixel = bgPixel;
			palette = bgPalette;
		}
	}
	
	if (sprite0hit && sprite0rendered)
	{
		if (mask.showBackground & mask.showSprites)
		{
			if (~(mask.showBackgroundLeft | mask.showSpritesLeft))
			{
				if (cycle >= 9 && cycle < 258)
				{
					status.sprite0hit = 1;
				}
			}
			else
			{
				if (cycle >= 1 && cycle < 258)
				{
					status.sprite0hit = 1;
				}
			}
		}
		
	}

	//this line will return the actual RGB color pixel that should be drawn
	uint32_t pixelColor = GetColorFromPalette(palette, pixel);

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

