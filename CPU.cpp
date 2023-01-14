#include "CPU.h"
#include "Bus.h"


CPU::CPU()
{
    
}

CPU::~CPU()
{
    
}

void CPU::write(uint8_t data, uint16_t addr)
{
    bus->cpuWrite(data,addr);
}

uint8_t CPU::read(uint16_t addr)
{
    return bus->cpuRead(addr);
}




//////////////////////////////////////////////////////
//ADDRESSING MODES

// Immediate Addressing mode - The operand will be in the next byte.
uint8_t CPU::IMM()
{
    operandAddr = pc++;
    return 0;
}


uint8_t CPU::IMP()
{
    return 0;
}

uint8_t CPU::REL()
{
    relAddr = read(pc++);
    if (relAddr & 0x80)
    {
        relAddr |= 0xFF00;
    }
}

uint8_t CPU::ABS()
{
    
}

uint8_t CPU::ZPP()
{
    
}

uint8_t CPU::IND()
{
    
}

uint8_t CPU::ABX()
{
    
}

uint8_t CPU::ABY()
{
    
}

uint8_t CPU::ZPX()
{
    
}

uint8_t CPU::ZPY()
{
    
}

uint8_t CPU::INX()
{
    
}

uint8_t CPU::INY()
{
    
}
