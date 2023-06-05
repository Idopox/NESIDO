#include "Mapper.h"



Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks)
{
    PRGBanks = prgBanks;
    CHRBanks = chrBanks;
}

Mapper::~Mapper()
{
}
