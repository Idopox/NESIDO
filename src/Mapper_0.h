#pragma once
#include "Mapper.h"

class Mapper_0 : public Mapper
{

public:
    Mapper_0(uint8_t prgBanks, uint8_t chrBanks);
    ~Mapper_0();

public:

    bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
    bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;


};