#pragma once

#include "CPU.h"

class Bus
{

public:
    Bus(/* args */);
    ~Bus();

public: 

    CPU cpu;

    uint8_t cpu_ram[2048];

public: 

    void cpu_write()

};
