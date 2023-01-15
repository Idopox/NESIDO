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
    return 0;
}

uint8_t CPU::ABS()
{
    uint8_t lowAddr = read(pc++);
    uint8_t highAddr = read(pc++);
    operandAddr = (highAddr << 8) | lowAddr;
    return 0;
}

uint8_t CPU::ZPG()
{
    operandAddr = read(pc++) & 0x00FF;
    return 0;
}

uint8_t CPU::IND()
{
    uint8_t lowAddr = read(pc++);
    uint8_t highAddr = read(pc++);
    uint16_t pointer = (highAddr << 8) | lowAddr;

    // handles a bug in 6502 cpu where if the low byte of the pointer is 0xFF, 
    // the high byte of the operand address is actually located at the start of the same page,
    // rather than the next page.
    if (lowAddr == 0x00FF) 
    {
        operandAddr = (read(pointer & 0xFF00) << 8) | read(pointer+0);
    }
    else
	{
		operandAddr = (read(pointer + 1) << 8) | read(pointer + 0);
	}
    
    return 0;
}

uint8_t CPU::ABX()
{
    uint8_t lowAddr = read(pc++);
    uint8_t highAddr = read(pc++);
    operandAddr = ((highAddr << 8) | lowAddr) + x;
    // checking if a page boundary happens
    if ((operandAddr & 0xFF00) != (highAddr << 8))
    {
        return 1;
    }
    return 0;
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
