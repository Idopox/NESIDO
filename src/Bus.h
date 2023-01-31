#pragma once

#include <array>
#include "CPU.h"

class Bus
{

public:
    Bus();
    ~Bus();

public: 

    CPU cpu;

    // uint8_t cpuRam[64*1024];
    std::array<uint8_t, 64*1024> cpuRam;

public: 

    void cpuWrite(uint8_t data, uint16_t addr);
    uint8_t cpuRead(uint16_t addr);

};
