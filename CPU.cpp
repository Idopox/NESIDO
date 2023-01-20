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
// To obtain the operand after call, read byte at pc
uint8_t CPU::IMM()
{
    operandAddr = pc++;
    return 0;
}

// Immediate Addressing mode - The operand is being specified within the opcode.
uint8_t CPU::IMP()
{
    implied = true;
    return 0;
}

// Relative Addressing mode - The operand is a signed 8 bit relative offset in the next byte.
// The operand will be saved in relAddr var
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
// The address will be kept in operandAddr var (no need to fetch after using this function)
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
// The address will be kept in operandAddr var (no need to fetch after using this function)
uint8_t CPU::ZPG(eIndexReg reg = CPU::eIndexReg::None)
{
    uint8_t indexed = (reg = CPU::eIndexReg::X) ? x : (reg == CPU::eIndexReg::Y) ? y : 0;
    operandAddr = (read(pc++) + indexed) & 0x00FF;
    return 0;
}

// Indirect Addressing mode - The operand is 16 bit address stored in address in the next two bytes.
// The address will be kept in operandAddr var (no need to fetch after using this function)
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
// The address will be kept in operandAddr var (no need to fetch after using this function)
uint8_t CPU::INX()
{
    uint16_t pointer = read(pc++);
    uint16_t low =  read((pointer+(uint16_t)x) & 0x00FF);
    uint16_t high = read((pointer+(uint16_t)x+1) & 0x00FF);
    operandAddr = (high << 8) |low;
    return 0;
}

// Indirect Indexed Addressing mode - The operand is 16 bit address stored in address in the zeropage. the offset in the next byte + y register
// The address will be kept in operandAddr var (no need to fetch after using this function)
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
    // implied = true if the addressing mode is implied and then the operand is a register.
    if (!implied)
    {
        return read(operandAddr);
    }
    return a; 
}


// INSTRUCTIONS

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
    write(status, sp--);
    return 0;
}

uint8_t CPU::PLA()
{
    a = read(sp++);
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 0;
}

uint8_t CPU::PLP()
{
    status = read(sp++);
    return 0;
}

uint8_t CPU::AND()
{
    uint8_t mem = fetch();
    a &= mem;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 1;
}

uint8_t CPU::EOR()
{
    uint8_t mem = fetch();
    a ^= mem;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 1;
}

uint8_t CPU::ORA()
{
    uint8_t mem = fetch();
    a |= mem;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 1;
}

uint8_t CPU::BIT()
{
    uint8_t mem = fetch();
    SetFlag(Z, (mem & a) == 0x00);
    SetFlag(N, mem & 0x80);
    SetFlag(V, mem & 0x40);
    return 0;
}

// ADC - Add Memory to Accumulator with Carry
//
//  The ADC instruction adds the contents of a memory location to the
//  accumulator together with the carry bit. If overflow occurs the
//  carry bit is set, this enables multiple byte addition to be performed
//
//  Flags affected:
//      N - Set if result is negative
//      Z - Set if result is zero
//      C - Set if carry from the addition
//      V - Set if overflow, i.e. sign bit is incorrect
uint8_t CPU::ADC()
{
    uint8_t mem = fetch();
    uint16_t result = (uint16_t)mem + uint16_t(a) + uint16_t(GetFlag(C));
    SetFlag(C, result > 255);
    SetFlag(V, (!((a ^ mem) & 0x80) && ((a^ result) & 0x80))); 
    a = 0x00FF & result;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);


    return 1;
}

// SBC - Subtract Memory from Accumulator with Borrow
//
// The SBC instruction subtracts the contents of a memory location
// from the accumulator together with the not of the carry bit.
// If overflow occurs the carry bit is set, this enables multiple byte
// subtraction to be performed.
//
// Flags affected:
// N - Set if result is negative
// Z - Set if result is zero
// C - Set if no borrow from the subtraction
// V - Set if overflow, i.e. sign bit is incorrect
uint8_t CPU::SBC()
{
    uint8_t mem = fetch();
    uint16_t result =  uint16_t(a) - (uint16_t)mem - uint16_t(1-GetFlag(C));
    SetFlag(C, result > 0x00);
    SetFlag(V, (((a ^ mem) & 0x80) && ((a ^ result) & 0x80))); 
    a = 0x00FF & result;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    return 1;
}

// CMP - Compare Memory and Accumulator
//
// The CMP instruction compares the contents of the accumulator to
// the contents of a memory location. If the accumulator is equal to or
// greater than the memory location, the carry flag is set. Otherwise,
// the carry flag is cleared.
//
// Flags affected:
// C - Set if the accumulator >= memory
// Z - Set if the accumulator == memory
// N - Set if the result is negative
uint8_t CPU::CMP()
{
    uint8_t mem = fetch();
    uint8_t cmp = a - mem;
    SetFlag(C, cmp >= 0x00);
    SetFlag(Z, cmp == 0x00);
    SetFlag(N, cmp & 0x80);
}

// CPX - Compare Memory and X
//
// The CPX instruction compares the contents of the x register to
// the contents of a memory location. If the x register is equal to or
// greater than the memory location, the carry flag is set. Otherwise,
// the carry flag is cleared.
//
// Flags affected:
// C - Set if the x  >= memory
// Z - Set if the x == memory
// N - Set if the result is negative
uint8_t CPU::CPX()
{
    uint8_t mem = fetch();
    uint8_t cmp = x - mem;
    SetFlag(C, cmp >= 0x00);
    SetFlag(Z, cmp == 0x00);
    SetFlag(N, cmp & 0x80);
    return 0;
}

// CPY - Compare Memory and Y
//
// The CPY instruction compares the contents of the y register to
// the contents of a memory location. If the y register is equal to or
// greater than the memory location, the carry flag is set. Otherwise,
// the carry flag is cleared.
//
// Flags affected:
// C - Set if the y  >= memory
// Z - Set if the y == memory
// N - Set if the result is negative
uint8_t CPU::CPY()
{
    uint8_t mem = fetch();
    uint8_t cmp = y - mem;
    SetFlag(C, cmp >= 0x00);
    SetFlag(Z, cmp == 0x00);
    SetFlag(N, cmp & 0x80);
    return 0;
}

// INC - Increment Memory by One
//
// The INC instruction increments the contents of a memory location by
// one.
//
// Flags affected:
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::INC()
{
    uint8_t res = fetch()+1;
    write(res, operandAddr);
    SetFlag(Z, res == 0x00);
    SetFlag(N, res & 0x80);
    return 0;
}

// INX - Increment X by One
//
// The INX instruction increments the contents of the X register by one.
//
// Flags affected:
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::INX()
{
    x++;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0x80);
    return 0;
}

// INY - Increment Y by One
//
// The INY instruction increments the contents of the Y register by one.
//
// Flags affected:
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::INY()
{
    y++;
    SetFlag(Z, y == 0x00);
    SetFlag(N, y & 0x80);
    return 0;
}

// DEC - Decrement Memory by One
//
// The DEC instruction decrements the contents of a memory location by
// one.
//
// Flags affected:
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::DEC()
{
    uint8_t res = fetch()-1;
    write(res, operandAddr);
    SetFlag(Z, res == 0x00);
    SetFlag(N, res & 0x80);
    return 0;
}

// DEX - Decrement X by One
//
// The DEX instruction decrements the contents of the X register by one.
//
// Flags affected:
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::DEX()
{
    x--;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0x80);
    return 0;
}

// DEY - Decrement Y by One
//
// The DEY instruction decrements the contents of the Y register by one.
//
// Flags affected:
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::DEY()
{
    y--;
    SetFlag(Z, y == 0x00);
    SetFlag(N, y & 0x80);
    return 0;
}

// ASL - Arithmetic Shift Left
//
// The ASL instruction performs a one-bit left shift of a memory location or the accumulator.
//
// Flags affected:
// C - Set if the most significant bit of the memory location or the accumulator before the shift
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::ASL()
{
    uint8_t memValue = fetch();
    uint8_t res = memValue << 1;
    SetFlag(C, memValue & 0x80);
    if (implied)
    {
        a = res;
        SetFlag(Z, a == 0x00);
        SetFlag(N, a & 0x80);
        implied = false;
    }
    else
    {
        write(res, operandAddr);
        SetFlag(Z, res == 0x00);
        SetFlag(N, res & 0x80);
    }
    return 0;
}

// LSR - Logical Shift Right
//
// The LSR instruction shifts all the bits in the operand to the right, with the bit that is shifted out becoming the new value of the carry flag.
//
// Flags affected:
// C - Set to the value of the least significant bit before the shift
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::LSR()
{
    uint8_t memValue = fetch();
    uint8_t res = memValue >> 1;
    SetFlag(C, memValue & 0x01);
    if (implied)
    {
        a = res;
        SetFlag(Z, a == 0x00);
        SetFlag(N, a & 0x80);
        implied = false;
    }
    else
    {
        write(res, operandAddr);
        SetFlag(Z, res == 0x00);
        SetFlag(N, res & 0x80);
    }
    return 0;
}

uint8_t CPU::ROL()
{
    uint8_t memValue = fetch();
    uint8_t res = memValue >> 1;
    SetFlag(C, memValue & 0x01);
    if (implied)
    {
        a = res;
        SetFlag(Z, a == 0x00);
        SetFlag(N, a & 0x80);
        implied = false;
    }
    else
    {
        write(res, operandAddr);
        SetFlag(Z, res == 0x00);
        SetFlag(N, res & 0x80);
    }
    return 0;
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




