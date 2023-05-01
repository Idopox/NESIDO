
#include "PPU.h"

PPU::PPU()
{
    
}

PPU::~PPU()
{
    
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
		ppuDataBuffer = ppuRead(address.fullAddr);

		if (address.fullAddr > 0x3F00) data = ppuDataBuffer;

		address.increment(control.bits.vram_addr_increment());
		break;
	}

	return data;
}

void PPU::cpuWrite(uint16_t addr, uint8_t  data)
{
    switch (addr)
	{
	case 0x0000: // Control
		control.reg = data;
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
		break;
	case 0x0006: // PPU Address
		address.update(data);
		break;
	case 0x0007: // PPU Data
		ppuWrite(address.fullAddr,data);

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
	
	cart->ppuWrite(data, addr);

	if (cart->ppuRead(data,addr))
	{
		
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{
		
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

void PPU::clock()
{

	// top left of the screen means we finished the vblank phase
	if (scanline == -1 && cycle == 1) 
	{
		status.verticalBlank = 1;
	}
	
	//starting vblank phase
	if (scanline == 241 && cycle == 1)
	{
		status.verticalBlank = 1;
		if (control.bits.enableNMI)
		{
			nmi = true;
		}
		
	}

    cycle++;
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
