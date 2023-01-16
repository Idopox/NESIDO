#pragma once

#include <stdint.h>

#include "Bus.h"

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
        D = (1 << 3), // Decimal mode (not supported in the nes cpu chip)
        B = (1 << 4), // Break Command
        U = (1 << 5), //unused bit
        V = (1 << 6), // Overflow
        N = (1 << 7) // Negative
    };
    enum eIndexReg{
        X, Y, None
    };

    uint8_t GetFlag(Flags f);
    void SetFlag(Flags f, bool state);

    uint8_t cycles = 0;
    uint8_t clockCount;
    uint16_t operandAddr;
    uint16_t relAddr;
    uint8_t fetch();

    Bus *bus = nullptr;

    void write(uint8_t data, uint16_t addr);
    uint8_t read(uint16_t addr);

    //ADDRESSING MODES
    uint8_t IMM();
    uint8_t IMP();
    uint8_t REL();
    uint8_t ABS(eIndexReg reg);
    uint8_t ZPG(eIndexReg reg);
    uint8_t IND();
    uint8_t ZPX();
    uint8_t ZPY();
    uint8_t INX();
    uint8_t INY();

    // OPCODES
    uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
    uint8_t STA(); uint8_t STX(); uint8_t STY();
    uint8_t TAX(); uint8_t TAY(); uint8_t TXA();
    uint8_t TYA(); uint8_t TSX(); uint8_t TXS();
    uint8_t PHA(); uint8_t PHP(); uint8_t PLA();
    uint8_t PLP(); uint8_t AND(); uint8_t EOR();
    uint8_t ORA(); uint8_t BIT(); uint8_t ADC();
    uint8_t SBC(); uint8_t CMP(); uint8_t CPX();
    uint8_t CPY(); uint8_t INC(); uint8_t INX();
    uint8_t INY(); uint8_t DEC(); uint8_t DEX();
    uint8_t DEY(); uint8_t ASL(); uint8_t LSR();
    uint8_t ROL(); uint8_t ROR(); uint8_t JMP();
    uint8_t JSR(); uint8_t RTS(); uint8_t BCC();
    uint8_t BCS(); uint8_t BEQ(); uint8_t BMI();
    uint8_t BNE(); uint8_t BPL(); uint8_t BVC();
    uint8_t BVS(); uint8_t CLC(); uint8_t CLD();
    uint8_t CLI(); uint8_t CLV(); uint8_t SEC();
    uint8_t SED(); uint8_t SEI(); uint8_t NOP();
    uint8_t BRK(); uint8_t RTI(); uint8_t XXX();

    





};