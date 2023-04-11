#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Mapper.h"

class Cartridge
{

public:
    Cartridge(const std::string &fileName);
    ~Cartridge();

public:

    enum MIRRORING{
        VERTICAL,
        HORIZONTAL,
    } mirror = HORIZONTAL;

private:

    uint8_t mapperID = 0;
    uint8_t PRGBanks = 0;
    uint8_t CHRBanks = 0;

    std::vector<uint8_t> PRGMemory;
    std::vector<uint8_t> CHRMemory;

    std::shared_ptr<Mapper> ptrMapper;

public:

    void cpuRead(uint8_t &data, uint16_t addr);
    void cpuWrite(uint8_t data, uint16_t addr);

    void ppuRead(uint8_t &data, uint16_t addr);
    void ppuWrite(uint8_t data, uint16_t addr);


};