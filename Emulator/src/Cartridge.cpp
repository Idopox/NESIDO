
#include <fstream>
#include <iostream>
#include <stdexcept>


#include "Cartridge.h"
#include "Mapper_0.h"


const char iNES_MAGIC[4] { 0x4E, 0x45, 0x53, 0x1A}; 
Cartridge::Cartridge(const std::string &fileName)
{
    
    //iNes format:
    struct Header
    {
        char magic[4]; //iNes format magic number
        uint8_t prg_rom_chunks; // Size of PRG ROM in 16 KB units
        uint8_t chr_rom_chunks; // Size of CHR ROM in 8 KB units (value 0 means the board uses CHR RAM)
        uint8_t mapper1;
        uint8_t mapper2;
        uint8_t prg_ram_size;
        uint8_t tv_system1;
        uint8_t tv_system2;
        char unused[5]; // Unused padding

    } header;

    std::ifstream file;
    file.open(fileName, std::ifstream::binary);
    if (file.is_open())
    {
        
        file.read((char*)&header, sizeof(Header));

        for (uint8_t i = 0; i < 4; i++)
        {
            if (header.magic[i] != iNES_MAGIC[i])
            {
                 throw std::invalid_argument( "File format not supported");
            }
        }
        

        //checking if a trainer exists (unused in the emulator). if it does, we read past it.
        if (header.mapper1 & 0x04)
        {
            file.seekg(512, std::ios_base::cur);
        }

        // checking which mapper we will use
        mapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

        PRGBanks = header.prg_rom_chunks;
        PRGMemory.resize(PRGBanks * 16384);
        file.read((char*)PRGMemory.data(), PRGMemory.size());

        CHRBanks = header.chr_rom_chunks;
        CHRMemory.resize(CHRBanks * 8192);
        file.read((char*)CHRMemory.data(), CHRMemory.size());

        // Load appropriate mapper
		switch (mapperID)
		{
		    case 0:
                ptrMapper = std::make_shared<Mapper_0>(PRGBanks, CHRBanks); 
                break;
		}

		file.close();
        

    }
    

}

Cartridge::~Cartridge()
{
    
}



void Cartridge::cpuRead(uint8_t &data, uint16_t addr)
{
    uint32_t mapped_addr = 0;
    if (ptrMapper->cpuMapRead(addr, mapped_addr))
    {
        data = PRGMemory[mapped_addr];
    }
    
}

void Cartridge::cpuWrite(uint8_t data, uint16_t addr)
{
    uint32_t mapped_addr = 0;
    if (ptrMapper->cpuMapWrite(addr, mapped_addr))
    {
        PRGMemory[mapped_addr] = data;
    }
}

bool Cartridge::ppuRead(uint8_t &data, uint16_t addr)
{
	uint32_t mapped_addr = 0;
	if (ptrMapper->ppuMapRead(addr, mapped_addr))
	{
		data = CHRMemory[mapped_addr];
        return true;
	}
    return false;
}

bool Cartridge::ppuWrite(uint8_t data, uint16_t addr)
{
	uint32_t mapped_addr = 0;
	if (ptrMapper->ppuMapRead(addr, mapped_addr))
	{
		CHRMemory[mapped_addr] = data;
        return true;
	}
    return false;
}
