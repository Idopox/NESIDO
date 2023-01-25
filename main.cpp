#include <sstream>
#include <iostream>
#include <bitset>

#include "CPU.h"
#include "Bus.h"

Bus nes;

std::string hex(uint32_t n, uint8_t d)
{
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
};

void DrawCpu()
{
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
    std::string status = "STATUS: ";
    std::cout <<"STATUS:" << std::endl;
    std::cout <<"N: ";
    nes.cpu.status & CPU::N ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"V: ";
    nes.cpu.status & CPU::V ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"U: ";
    nes.cpu.status & CPU::U ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"B: ";
    nes.cpu.status & CPU::B ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"D: ";
    nes.cpu.status & CPU::D ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"I: ";
    nes.cpu.status & CPU::I ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"Z: ";
    nes.cpu.status & CPU::Z ? std::cout << 1 << "   " : std::cout << 0 << "   ";
    std::cout <<"C: ";
    nes.cpu.status & CPU::C ? std::cout << 1 << std::endl: std::cout << 0 << std::endl;
    std::cout << "PC: $" << hex(nes.cpu.pc, 4) << std::endl;
    std::cout << "A: $" << hex(nes.cpu.a, 2)<< std::endl;
    std::cout << "X: $" << hex(nes.cpu.x, 2) << std::endl;
    std::cout << "Y: $" << hex(nes.cpu.y, 2) << std::endl;
    std::cout << "SP: $" << hex(nes.cpu.sp, 2) << std::endl;
    std::cout << std::endl;
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
}

int main(int argc, char const *argv[])
{
    
    std::stringstream ss;
    ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
    uint16_t nOffset = 0x8000;
    while (!ss.eof())
    {
        std::string b;
        ss >> b;
        nes.cpuRam[nOffset++] = (uint8_t)std::stoul(b, nullptr, 16);
    }

    // Set Reset Vector
    nes.cpuRam[0xFFFC] = 0x00;
    nes.cpuRam[0xFFFD] = 0x80;

    // Reset
    nes.cpu.reset();

    while (true)
    {
        nes.cpu.clock();
        DrawCpu();
    }
    


    return 0;
}
