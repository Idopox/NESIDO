#pragma once

#include "CPU.h"

class Bus
{

public:
    Bus();
    ~Bus();

public: 

    CPU cpu;

    uint8_t cpuRam[2048];

public: 

    void cpuWrite(uint8_t data, uint16_t addr);
    uint8_t cpuRead(uint16_t addr);

};
