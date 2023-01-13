#pragma once

#include <stdint.h>

class CPU
{
    
public:

    CPU();
    ~CPU();

public:
    void IRQ(); // a non-maskable interrupt
    void NMI(); // a maskable interrupt
    void reset(); // a reset signal interrupt

    void clock(); // perform one clock cycle

    void ConnectBus(Bus *b)
    {
        bus = b;
    }

private:

    uint8_t a = 0x00; // Accumulator register
    uint8_t x = 0x00; // General purpose register X
    uint8_t y = 0x00; // General purpose register Y

    uint16_t pc = 0x0000; // Program Counter
    uint8_t sp = 0x00; // Stack Pointer register

    uint8_t status = 0x00; // Processor status

    enum Flags{
        C = (1 << 0), // Carry
        Z = (1 << 1), // Zero
        I = (1 << 2), // Interrupt disable
        D = (1 << 3), // Decimal mode
        B = (1 << 4), // Break Command
        U = (1 << 5), //unused bit
        V = (1 << 6), // Overflow
        N = (1 << 7) // Negative
    }

    Bus *bus = nullptr;

    void write(uint16_t data, uint16_t addr);
    uint16_t read(uint16_t addr);

    //ADDRESSING MODES
    uint8_t ACC();
    uint8_t IMM();
    uint8_t IMP();
    uint8_t REL();
    uint8_t ABS();
    uint8_t ZPP();
    uint8_t IND();
    uint8_t ABX();
    uint8_t ABY();
    uint8_t ZPX();
    uint8_t ZPY();
    uint8_t INX();
    uint8_t INY();

    // OPCODES
    uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
    uint8_t STA(); uint8_t STX(); uint8_t STY();
    uint8_t TAX(); uint8_t TAY(); uint8_t TXA();
    





}