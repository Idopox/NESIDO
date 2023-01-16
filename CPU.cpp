#include "CPU.h"
#include "Bus.h"


CPU::CPU()
{

}

CPU::~CPU()
{
    
}

uint8_t CPU::GetFlag(Flags f)
{
    return (status & f) > 0;
}

void CPU::SetFlag(Flags f, bool state)
{
    if (state)
        status |= f;
    else
        status &= ~f;
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

// Immediate Addressing mode - The operand is being specified within the opcode.
uint8_t CPU::IMP()
{
    operandAddr = -1;
    return 0;
}

// Relative Addressing mode - The operand is a signed 8 bit relative offset in the next byte.
uint8_t CPU::REL()
{
    relAddr = read(pc++);
    if (relAddr & 0x80)
    {
        relAddr |= 0xFF00;
    }
    return 0;
}

// Absolute Addressing mode - The operand is 16 bit address in the next two bytes.
uint8_t CPU::ABS(eIndexReg reg = CPU::eIndexReg::None)
{
    uint8_t lowAddr = read(pc++);
    uint8_t highAddr = read(pc++);
    uint8_t indexed = (reg = CPU::eIndexReg::X) ? x : (reg == CPU::eIndexReg::Y) ? y : 0;

    operandAddr = (highAddr << 8) | lowAddr + indexed;

    if (reg != CPU::eIndexReg::None)
    {
        // checking if a page boundary happens, if happens - returning 1, indicating that an extra cycle is needed.
        if ((operandAddr & 0xFF00) != (highAddr << 8))
        {
            return 1;
        }
    }

    return 0;
}

// Zero Page Addressing mode - The operand is 8 bit offset in the zero page (0x00??).
uint8_t CPU::ZPG(eIndexReg reg = CPU::eIndexReg::None)
{
    uint8_t indexed = (reg = CPU::eIndexReg::X) ? x : (reg == CPU::eIndexReg::Y) ? y : 0;
    operandAddr = (read(pc++) + indexed) & 0x00FF;
    return 0;
}

// Indirect Addressing mode - The operand is 16 bit address stored in address in the next two bytes.
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

// Indexed Indirect Addressing mode - The operand is 16 bit address stored in address in the zeropage. the offset in the next byte + x register
uint8_t CPU::INX()
{
    uint16_t pointer = read(pc++);
    uint16_t low =  read((pointer+(uint16_t)x) & 0x00FF);
    uint16_t high = read((pointer+(uint16_t)x+1) & 0x00FF);
    operandAddr = (high << 8) |low;
    return 0;
}

// Indirect Indexed Addressing mode - The operand is 16 bit address stored in address in the zeropage. the offset in the next byte + y register
uint8_t CPU::INY()
{
    uint16_t pointer = read(pc++);
    uint16_t low =  read(pointer & 0x00FF);
    uint16_t high = read((pointer+1) & 0x00FF);
    operandAddr = (high << 8) |low;
    operandAddr += y;

    if ((operandAddr & 0xFF00) != (high << 8))
    {
        return 1;
    }
    return 0;
}

// Extracting the data used by the instruction
uint8_t CPU::fetch()
{
    // operandAddr = -1 if the addressing mode is implied and then the operand is a register.
    if (operandAddr != -1)
    {
        return read(operandAddr);
    }
    return a; 
}


// OPCODES

uint8_t CPU::LDA()
{
    a = fetch();
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 1;
}

uint8_t CPU::LDX()
{
    x = fetch();
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 1;
}

uint8_t CPU::LDY()
{
    y = fetch();
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 1;
}

uint8_t CPU::STA()
{
    write(a,operandAddr);
    return 0;
}

uint8_t CPU::STX()
{
    write(x,operandAddr);
    return 0;
}

uint8_t CPU::STY()
{
    write(y,operandAddr);
    return 0;
}

uint8_t CPU::TAX()
{
    x = a;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0x80);
    return 0;
}

uint8_t CPU::TAY()
{
    y = a;
    SetFlag(Z, y == 0x00);
    SetFlag(N, y & 0x80);
    return 0;
}

uint8_t CPU::TXA()
{
    a = x;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 0;
}

uint8_t CPU::TYA()
{
    a = y;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 0;
}

uint8_t CPU::TSX()
{
    x = sp;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0x80);
    return 0;
}

uint8_t CPU::TXS()
{
    sp = x;
    SetFlag(Z, sp == 0x00);
    SetFlag(N, sp & 0x80);
    return 0;
}

uint8_t CPU::PHA()
{
    write(a,0x0100 + sp--);
    return 0;
}

uint8_t CPU::PHP()
{
    
}

uint8_t CPU::PLA()
{
    
}

uint8_t CPU::PLP()
{
    
}

uint8_t CPU::AND()
{
    
}

uint8_t CPU::EOR()
{
    
}

uint8_t CPU::ORA()
{
    
}

uint8_t CPU::BIT()
{
    
}

uint8_t CPU::ADC()
{
    
}

uint8_t CPU::SBC()
{
    
}

uint8_t CPU::CMP()
{
    
}

uint8_t CPU::CPX()
{
    
}

uint8_t CPU::CPY()
{
    
}

uint8_t CPU::INC()
{
    
}

uint8_t CPU::INX()
{
    
}

uint8_t CPU::INY()
{
    
}

uint8_t CPU::DEC()
{
    
}

uint8_t CPU::DEX()
{
    
}

uint8_t CPU::DEY()
{
    
}

uint8_t CPU::ASL()
{
    
}

uint8_t CPU::LSR()
{
    
}

uint8_t CPU::ROL()
{
    
}

uint8_t CPU::ROR()
{
    
}

uint8_t CPU::JMP()
{
    
}

uint8_t CPU::JSR()
{
    
}

uint8_t CPU::RTS()
{
    
}

uint8_t CPU::BCC()
{
    
}

uint8_t CPU::BCS()
{
    
}

uint8_t CPU::BEQ()
{
    
}

uint8_t CPU::BMI()
{
    
}

uint8_t CPU::BNE()
{
    
}

uint8_t CPU::BPL()
{
    
}

uint8_t CPU::BVC()
{
    
}

uint8_t CPU::BVS()
{
    
}

uint8_t CPU::CLC()
{
    
}

uint8_t CPU::CLD()
{
    
}

uint8_t CPU::CLI()
{
    
}

uint8_t CPU::CLV()
{
    
}

uint8_t CPU::SEC()
{
    
}

uint8_t CPU::SED()
{
    
}

uint8_t CPU::SEI()
{
    
}

uint8_t CPU::NOP()
{
    
}

uint8_t CPU::BRK()
{
    
}

uint8_t CPU::RTI()
{
    
}

uint8_t CPU::XXX()
{
    
}




