#include "CPU.h"
#include "Bus.h"


CPU::CPU()
{
	using a = CPU;
	lookup =
    {
		{&a::BRK, &a::IMM, 7 },{&a::ORA, &a::IDX, 6 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 3 },{&a::ORA, &a::ZP0, 3 },{&a::ASL, &a::ZP0, 5 },{&a::XXX, &a::IMP, 5 },{&a::PHP, &a::IMP, 3 },{&a::ORA, &a::IMM, 2 },{&a::ASL, &a::IMP, 2 },{&a::XXX, &a::IMP, 2 },{&a::NOP, &a::IMP, 4 },{&a::ORA, &a::AB0, 4 },{&a::ASL, &a::AB0, 6 },{&a::XXX, &a::IMP, 6 },
		{&a::BPL, &a::REL, 2 },{&a::ORA, &a::IDY, 5 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 4 },{&a::ORA, &a::ZPX, 4 },{&a::ASL, &a::ZPX, 6 },{&a::XXX, &a::IMP, 6 },{&a::CLC, &a::IMP, 2 },{&a::ORA, &a::ABY, 4 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 7 },{&a::NOP, &a::IMP, 4 },{&a::ORA, &a::ABX, 4 },{&a::ASL, &a::ABX, 7 },{&a::XXX, &a::IMP, 7 },
		{&a::JSR, &a::AB0, 6 },{&a::AND, &a::IDX, 6 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::BIT, &a::ZP0, 3 },{&a::AND, &a::ZP0, 3 },{&a::ROL, &a::ZP0, 5 },{&a::XXX, &a::IMP, 5 },{&a::PLP, &a::IMP, 4 },{&a::AND, &a::IMM, 2 },{&a::ROL, &a::IMP, 2 },{&a::XXX, &a::IMP, 2 },{&a::BIT, &a::AB0, 4 },{&a::AND, &a::AB0, 4 },{&a::ROL, &a::AB0, 6 },{&a::XXX, &a::IMP, 6 },
		{&a::BMI, &a::REL, 2 },{&a::AND, &a::IDY, 5 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 4 },{&a::AND, &a::ZPX, 4 },{&a::ROL, &a::ZPX, 6 },{&a::XXX, &a::IMP, 6 },{&a::SEC, &a::IMP, 2 },{&a::AND, &a::ABY, 4 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 7 },{&a::NOP, &a::IMP, 4 },{&a::AND, &a::ABX, 4 },{&a::ROL, &a::ABX, 7 },{&a::XXX, &a::IMP, 7 },
		{&a::RTI, &a::IMP, 6 },{&a::EOR, &a::IDX, 6 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 3 },{&a::EOR, &a::ZP0, 3 },{&a::LSR, &a::ZP0, 5 },{&a::XXX, &a::IMP, 5 },{&a::PHA, &a::IMP, 3 },{&a::EOR, &a::IMM, 2 },{&a::LSR, &a::IMP, 2 },{&a::XXX, &a::IMP, 2 },{&a::JMP, &a::AB0, 3 },{&a::EOR, &a::AB0, 4 },{&a::LSR, &a::AB0, 6 },{&a::XXX, &a::IMP, 6 },
		{&a::BVC, &a::REL, 2 },{&a::EOR, &a::IDY, 5 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 4 },{&a::EOR, &a::ZPX, 4 },{&a::LSR, &a::ZPX, 6 },{&a::XXX, &a::IMP, 6 },{&a::CLI, &a::IMP, 2 },{&a::EOR, &a::ABY, 4 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 7 },{&a::NOP, &a::IMP, 4 },{&a::EOR, &a::ABX, 4 },{&a::LSR, &a::ABX, 7 },{&a::XXX, &a::IMP, 7 },
		{&a::RTS, &a::IMP, 6 },{&a::ADC, &a::IDX, 6 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 3 },{&a::ADC, &a::ZP0, 3 },{&a::ROR, &a::ZP0, 5 },{&a::XXX, &a::IMP, 5 },{&a::PLA, &a::IMP, 4 },{&a::ADC, &a::IMM, 2 },{&a::ROR, &a::IMP, 2 },{&a::XXX, &a::IMP, 2 },{&a::JMP, &a::IND, 5 },{&a::ADC, &a::AB0, 4 },{&a::ROR, &a::AB0, 6 },{&a::XXX, &a::IMP, 6 },
		{&a::BVS, &a::REL, 2 },{&a::ADC, &a::IDY, 5 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 4 },{&a::ADC, &a::ZPX, 4 },{&a::ROR, &a::ZPX, 6 },{&a::XXX, &a::IMP, 6 },{&a::SEI, &a::IMP, 2 },{&a::ADC, &a::ABY, 4 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 7 },{&a::NOP, &a::IMP, 4 },{&a::ADC, &a::ABX, 4 },{&a::ROR, &a::ABX, 7 },{&a::XXX, &a::IMP, 7 },
		{&a::NOP, &a::IMP, 2 },{&a::STA, &a::IDX, 6 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 6 },{&a::STY, &a::ZP0, 3 },{&a::STA, &a::ZP0, 3 },{&a::STX, &a::ZP0, 3 },{&a::XXX, &a::IMP, 3 },{&a::DEY, &a::IMP, 2 },{&a::NOP, &a::IMP, 2 },{&a::TXA, &a::IMP, 2 },{&a::XXX, &a::IMP, 2 },{&a::STY, &a::AB0, 4 },{&a::STA, &a::AB0, 4 },{&a::STX, &a::AB0, 4 },{&a::XXX, &a::IMP, 4 },
		{&a::BCC, &a::REL, 2 },{&a::STA, &a::IDY, 6 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 6 },{&a::STY, &a::ZPX, 4 },{&a::STA, &a::ZPX, 4 },{&a::STX, &a::ZPY, 4 },{&a::XXX, &a::IMP, 4 },{&a::TYA, &a::IMP, 2 },{&a::STA, &a::ABY, 5 },{&a::TXS, &a::IMP, 2 },{&a::XXX, &a::IMP, 5 },{&a::NOP, &a::IMP, 5 },{&a::STA, &a::ABX, 5 },{&a::XXX, &a::IMP, 5 },{&a::XXX, &a::IMP, 5 },
		{&a::LDY, &a::IMM, 2 },{&a::LDA, &a::IDX, 6 },{&a::LDX, &a::IMM, 2 },{&a::XXX, &a::IMP, 6 },{&a::LDY, &a::ZP0, 3 },{&a::LDA, &a::ZP0, 3 },{&a::LDX, &a::ZP0, 3 },{&a::XXX, &a::IMP, 3 },{&a::TAY, &a::IMP, 2 },{&a::LDA, &a::IMM, 2 },{&a::TAX, &a::IMP, 2 },{&a::XXX, &a::IMP, 2 },{&a::LDY, &a::AB0, 4 },{&a::LDA, &a::AB0, 4 },{&a::LDX, &a::AB0, 4 },{&a::XXX, &a::IMP, 4 },
		{&a::BCS, &a::REL, 2 },{&a::LDA, &a::IDY, 5 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 5 },{&a::LDY, &a::ZPX, 4 },{&a::LDA, &a::ZPX, 4 },{&a::LDX, &a::ZPY, 4 },{&a::XXX, &a::IMP, 4 },{&a::CLV, &a::IMP, 2 },{&a::LDA, &a::ABY, 4 },{&a::TSX, &a::IMP, 2 },{&a::XXX, &a::IMP, 4 },{&a::LDY, &a::ABX, 4 },{&a::LDA, &a::ABX, 4 },{&a::LDX, &a::ABY, 4 },{&a::XXX, &a::IMP, 4 },
		{&a::CPY, &a::IMM, 2 },{&a::CMP, &a::IDX, 6 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::CPY, &a::ZP0, 3 },{&a::CMP, &a::ZP0, 3 },{&a::DEC, &a::ZP0, 5 },{&a::XXX, &a::IMP, 5 },{&a::INY, &a::IMP, 2 },{&a::CMP, &a::IMM, 2 },{&a::DEX, &a::IMP, 2 },{&a::XXX, &a::IMP, 2 },{&a::CPY, &a::AB0, 4 },{&a::CMP, &a::AB0, 4 },{&a::DEC, &a::AB0, 6 },{&a::XXX, &a::IMP, 6 },
		{&a::BNE, &a::REL, 2 },{&a::CMP, &a::IDY, 5 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 4 },{&a::CMP, &a::ZPX, 4 },{&a::DEC, &a::ZPX, 6 },{&a::XXX, &a::IMP, 6 },{&a::CLD, &a::IMP, 2 },{&a::CMP, &a::ABY, 4 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 7 },{&a::NOP, &a::IMP, 4 },{&a::CMP, &a::ABX, 4 },{&a::DEC, &a::ABX, 7 },{&a::XXX, &a::IMP, 7 },
		{&a::CPX, &a::IMM, 2 },{&a::SBC, &a::IDX, 6 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::CPX, &a::ZP0, 3 },{&a::SBC, &a::ZP0, 3 },{&a::INC, &a::ZP0, 5 },{&a::XXX, &a::IMP, 5 },{&a::INX, &a::IMP, 2 },{&a::SBC, &a::IMM, 2 },{&a::NOP, &a::IMP, 2 },{&a::SBC, &a::IMP, 2 },{&a::CPX, &a::AB0, 4 },{&a::SBC, &a::AB0, 4 },{&a::INC, &a::AB0, 6 },{&a::XXX, &a::IMP, 6 },
		{&a::BEQ, &a::REL, 2 },{&a::SBC, &a::IDY, 5 },{&a::XXX, &a::IMP, 2 },{&a::XXX, &a::IMP, 8 },{&a::NOP, &a::IMP, 4 },{&a::SBC, &a::ZPX, 4 },{&a::INC, &a::ZPX, 6 },{&a::XXX, &a::IMP, 6 },{&a::SED, &a::IMP, 2 },{&a::SBC, &a::ABY, 4 },{&a::NOP, &a::IMP, 2 },{&a::XXX, &a::IMP, 7 },{&a::NOP, &a::IMP, 4 },{&a::SBC, &a::ABX, 4 },{&a::INC, &a::ABX, 7 },{&a::XXX, &a::IMP, 7 },	
    };
}

CPU::~CPU()
{
    
}


void CPU::clock()
{
    clockCount++;
    if (cycles == 0)
    {
        uint8_t opcode = read(pc++);

        cycles = lookup[opcode].cycles;

        SetFlag(U,true);

        uint8_t add_cycle1 = (this->*lookup[opcode].addrmode)();
        uint8_t add_cycle2 = (this->*lookup[opcode].operate)();

        cycles += add_cycle1 & add_cycle2;

        SetFlag(U,true);
        
    }
    cycles--;
    
}

// RESET - Reset the CPU
//
// The RESET instruction is used to reset the CPU. It sets the program
// counter to the address stored at fixed location 0xFFFC, and sets the
// status register to 0x00 with the U flag set. The A, X, and Y registers
// are set to 0, and the stack pointer is set to 0xFD.
//
// Flags affected:
// B - cleared
// I - cleared
// U - set
void CPU::reset()
{
    // Get address to set program counter to
	operandAddr = 0xFFFC;
	uint16_t lowAddr = read(operandAddr + 0);
	uint16_t highAddr = read(operandAddr + 1);
    pc = (highAddr << 8) | lowAddr;
    a, x, y = 0;
    status = 0x00 | I | U;
    sp = 0xFD;

    operandAddr = 0x0000;
    relAddr = 0x0000;

    cycles = 8;

}

// IRQ - Interrupt Request
//
// The IRQ instruction is used to request an interrupt. It pushes the program
// counter and the status register onto the stack, and then sets the program
// counter to the address stored at fixed location 0xFFFE. The I flag is set,
// the B flag is cleared, and the U flag is set
//
// Flags affected:
// B - cleared
// I - set
// U - set
void CPU::IRQ()
{
    if (!GetFlag(I))
    {
        write((pc >> 8) & 0x00FF, 0x0100 + sp--);
		write(pc & 0x00FF, 0x0100 + sp--);

        SetFlag(B, 0);
		SetFlag(U, 1);
		SetFlag(I, 1);
		write(status, 0x0100 + sp--);

        // Read new program counter location from fixed address
		operandAddr = 0xFFFE;
		uint16_t lo = read(operandAddr + 0);
		uint16_t hi = read(operandAddr + 1);
		pc = (hi << 8) | lo;

		cycles = 7;
    }
    
}

// NMI - Non-maskable Interrupt
//
// The NMI interrupt is triggered by a specific event, such as a button press,
// that cannot be ignored by the processor. The NMI interrupt pushes the program
// counter and status register onto the stack, sets the break flag, sets the
// unused flag, sets the interrupt disable flag, and sets the program counter to
// the memory location at 0xFFFA.
//
// Flags affected:
// B - Set to 0
// U - Set to 1
// I - Set to 1
void CPU::NMI()
{
    write((pc >> 8) & 0x00FF, 0x0100 + sp--);
	write(pc & 0x00FF, 0x0100 + sp--);

    SetFlag(B, 0);
    SetFlag(U, 1);
    SetFlag(I, 1);
    write(status, 0x0100 + sp--);

    // Read new program counter location from fixed address
    operandAddr = 0xFFFA;
    uint16_t lo = read(operandAddr + 0);
    uint16_t hi = read(operandAddr + 1);
    pc = (hi << 8) | lo;

    cycles = 8;
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
    uint8_t indexed = (reg == CPU::eIndexReg::X) ? x : (reg == CPU::eIndexReg::Y) ? y : 0;

    operandAddr = ((highAddr << 8) | lowAddr) + indexed;

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

//Non indexed Absolute addressing mode
uint8_t CPU::AB0()
{
    return ABS(CPU::eIndexReg::None);
}

//X indexed Absolute addressing mode
uint8_t CPU::ABX()
{
    return ABS(CPU::eIndexReg::X);
}

//Y indexed Absolute addressing mode
uint8_t CPU::ABY()
{
    return ABS(CPU::eIndexReg::Y);
}

// Zero Page Addressing mode - The operand is 8 bit offset in the zero page (0x00??).
// The address will be kept in operandAddr var (no need to fetch after using this function)
uint8_t CPU::ZPG(eIndexReg reg = CPU::eIndexReg::None)
{
    uint8_t indexed = (reg == CPU::eIndexReg::X) ? x : (reg == CPU::eIndexReg::Y) ? y : 0;
    operandAddr = (read(pc++) + indexed) & 0x00FF;
    return 0;
}

//Non indexed Zero Page addressing mode
uint8_t CPU::ZP0()
{
    return ZPG(CPU::eIndexReg::None);
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

//X indexed Zero Page addressing mode
uint8_t CPU::ZPX()
{
    return ZPG(CPU::eIndexReg::X);
}

//Y indexed Zero Page addressing mode
uint8_t CPU::ZPY()
{
    return ZPG(CPU::eIndexReg::Y);
}

// Indexed Indirect Addressing mode - The operand is 16 bit address stored in address in the zeropage. the offset in the next byte + x register
// The address will be kept in operandAddr var (no need to fetch after using this function)
uint8_t CPU::IDX()
{
    uint16_t pointer = read(pc++);
    uint16_t low =  read((pointer+(uint16_t)x) & 0x00FF);
    uint16_t high = read((pointer+(uint16_t)x+1) & 0x00FF);
    operandAddr = (high << 8) |low;
    return 0;
}

// Indirect Indexed Addressing mode - The operand is 16 bit address stored in address in the zeropage. the offset in the next byte + y register
// The address will be kept in operandAddr var (no need to fetch after using this function)
uint8_t CPU::IDY()
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

void CPU::branch()
{
    cycles++;
    uint8_t page = pc & 0xFF00;
    pc += relAddr;
    if (page != (pc & 0xFF00))
    {
        cycles++;
    }
    return;
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
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0x80);
    return 1;
}

uint8_t CPU::LDY()
{
    y = fetch();
    SetFlag(Z, y == 0x00);
    SetFlag(N, y & 0x80);
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
    implied = false;
    return 0;
}

uint8_t CPU::TAY()
{
    y = a;
    SetFlag(Z, y == 0x00);
    SetFlag(N, y & 0x80);
    implied = false;
    return 0;
}

uint8_t CPU::TXA()
{
    a = x;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    implied = false;
    return 0;
}

uint8_t CPU::TYA()
{
    a = y;
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    implied = false;
    return 0;
}

uint8_t CPU::TSX()
{
    x = sp;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0x80);
    implied = false;
    return 0;
}

uint8_t CPU::TXS()
{
    sp = x;
    implied = false;
    return 0;
}

uint8_t CPU::PHA()
{
    write(a,0x100 + sp--);
    implied = false;
    return 0;
}

// PHP
// Push Processor Status on Stack

// The status register will be pushed with the break
// flag and bit 5 set to 1.
uint8_t CPU::PHP()
{
    write(status | B | U,0x100 + sp--);
    SetFlag(B, 0);
	SetFlag(U, 0);
    implied = false;
    return 0;
}

uint8_t CPU::PLA()
{
    a = read(0x100 + ++sp);
    SetFlag(Z, a == 0x00);
    SetFlag(N, a & 0x80);
    implied = false;
    return 0;
}

// PLP - Pull Processor Status
//
// The PLP instruction pulls a byte from the stack and sets the status register
// to the value of that byte. Bits 4 and 5 of the status register are ignored.
//
// Flags affected:
// All status flags are updated according to the value pulled from the stack,
uint8_t CPU::PLP()
{
    uint8_t pulledStatus = read(0x100 + ++sp);
    status = (pulledStatus & 0xEF) | (status & 0x10) | 0x20;

    implied = false;
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
    SetFlag(C, !(result & 0xFF00));
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
    SetFlag(C, a >= mem);
    SetFlag(Z, cmp == 0x00);
    SetFlag(N, cmp & 0x80);
    return 1;
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
    SetFlag(C, x >= mem);
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
    SetFlag(C, y >= mem);
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
    implied = false;
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
    implied = false;
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
    implied = false;
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
    implied = false;
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
        implied = false;
    }
    else
        write(res, operandAddr);

    SetFlag(Z, res == 0x00);
    SetFlag(N, res & 0x80);
    return 0;
}

// ROL - Rotate Left
//
// The ROL instruction shifts all the bits in a memory location or accumulator one place left.
// The bit that was shifted out is replaced by the current carry flag.
// The carry flag is set to the value of the bit that was shifted out.
//
// Flags affected:
// C - Set to the value of the bit shifted out
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::ROL()
{
    // fetching either the accumulator or a value at memory
    uint8_t memValue = fetch();
    uint8_t res = memValue << 1;

    // set the least significant bit to the current carry flag
    res |= GetFlag(C);
    SetFlag(C, memValue & 0x80);
    if (implied)
    {
        a = res;
        implied = false;
    }
    else
        write(res, operandAddr);
        
    SetFlag(Z, res == 0x00);
    SetFlag(N, res & 0x80);
    return 0;
}

// ROR - Rotate Right
//
// The ROR instruction shifts all the bits right one time. The carry flag
// is set to the value of the last bit shifted out of the operand. The
// first bit shifted in is taken from the carry flag.
//
// Flags affected:
// C - Set to the value of the last bit shifted out of the operand
// Z - Set if the result is zero
// N - Set if the result is negative
uint8_t CPU::ROR()
{
    // fetching either the accumulator or a value at memory
    uint8_t memValue = fetch();

    // set the most significant bit to the current carry flag
    uint8_t res = (GetFlag(C) << 7) | (memValue >> 1);

    SetFlag(C, memValue & 0x01);
    if (implied)
    {
        a = res;
        implied = false;
    }
    else
        write(res, operandAddr);
        
    SetFlag(Z, res == 0x00);
    SetFlag(N, res & 0x80);
    return 0;
}

// JMP - Jump to new Location
//
// The JMP instruction sets the program counter to a new memory location.
// This instruction can be used to change the flow of program execution
// by jumping to a new location in memory.
//
// Flags affected: None
uint8_t CPU::JMP()
{
    pc = operandAddr;
    return 0;
}

// JSR - Jump to Subroutine
//
// The JSR instruction pushes the address (minus one) of the return point on the stack
// and then sets the program counter to the target memory address.
// This allows the program to execute a subroutine at the target location and return to the instruction following JSR.
//
// Flags affected: None
uint8_t CPU::JSR()
{
    pc--;
    write((pc>>8) & 0x00FF ,0x100 + sp--);
    write(pc & 0x00FF ,0x100 + sp--);
    pc = operandAddr;
    return 0;
}

// RTS - Return from Subroutine
//
// The RTS instruction is used at the end of a subroutine to return to the
// calling routine. The program counter (PC) is loaded with the contents
// of the stack, effectively returning execution to the instruction
// following the JSR instruction that began the subroutine.
//
// Flags affected: None
uint8_t CPU::RTS()
{
    uint8_t lowAddr = read(0x100 + ++sp);
    uint8_t highAddr = read(0x100 + ++sp);
    pc = (highAddr << 8) | lowAddr;
    pc++;
    implied = false;
    return 0;
}

uint8_t CPU::BCC()
{
    if (!GetFlag(C))
    {
        branch();
    }
    return 0;
    
}

uint8_t CPU::BCS()
{
    if (GetFlag(C))
    {
        branch();
    }
    return 0;
}

uint8_t CPU::BEQ()
{
    if (GetFlag(Z))
    {
        branch();
    }
    return 0;
}

uint8_t CPU::BMI()
{
    if (GetFlag(N))
    {
        branch();
    }
    return 0;
}

uint8_t CPU::BNE()
{
    if (!GetFlag(Z))
    {
        branch();
    }
    return 0;
}

uint8_t CPU::BPL()
{
    if (!GetFlag(N))
    {
        branch();
    }
    return 0;
}

uint8_t CPU::BVC()
{
    if (!GetFlag(V))
    {
        branch();
    }
    return 0;
}

uint8_t CPU::BVS()
{
    if (GetFlag(V))
    {
        branch();
    }
    return 0;
}

uint8_t CPU::CLC()
{
    SetFlag(C,false);
    implied = false;
    return 0;
}

uint8_t CPU::CLD()
{
    SetFlag(D,false);
    implied = false;
    return 0;
}

uint8_t CPU::CLI()
{
    SetFlag(I,false);
    implied = false;
    return 0;
}

uint8_t CPU::CLV()
{
    SetFlag(V,false);
    implied = false;
    return 0;
}

uint8_t CPU::SEC()
{
    SetFlag(C,true);
    implied = false;
    return 0;
}

uint8_t CPU::SED()
{
    SetFlag(D,true);
    implied = false;
    return 0;
}

uint8_t CPU::SEI()
{
    SetFlag(I,true);
    implied = false;
    return 0;
}

uint8_t CPU::NOP()
{
    implied = false;
    return 0;
}

uint8_t CPU::BRK()
{
    pc++;

    SetFlag(I,true);
    write((pc >> 8) & 0x00FF, 0x100 + sp--);
    write(pc & 0x00FF, 0x100 + sp--);

    SetFlag(B,true);
    write(status, sp--);
    SetFlag(B,false);

    uint8_t lowAddr = read(0xFFFE);
    uint8_t highAddr = read(0xFFFF);
    pc = (highAddr << 8)| lowAddr;
    
    implied = false;
    return 0;
}

uint8_t CPU::RTI()
{
    status = read(0x100 + ++sp);
    status &= ~B;
	status &= ~U;
    
    uint8_t lowAddr = read(0x100 + ++sp);
    uint8_t highAddr = read(0x100 + ++sp);
    pc = (highAddr << 8) | lowAddr;
    implied = false;
    return 0;
}

uint8_t CPU::XXX()
{
    return 0;
}

//a function used mainly for debugging
bool CPU::complete()
{
	return cycles == 0;
}




