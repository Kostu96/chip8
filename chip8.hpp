#pragma once
#include "types.hpp"

class Chip8
{
public:
    Chip8() = default;

    void loadProgram(u8* memory) { m_memory = memory; }
    void reset();
    void clock();

    const u8* getScreenMemory() const { return Screen; }

    Chip8(const Chip8&) = delete;
    Chip8& operator=(const Chip8&) = delete;
private:
    u8 Screen[64*32];
    u16 Stack[16];
    u8 GPR[16];
    u16 I;
    u16 PC : 12;
    u8 SP : 4;
    u8 DT;
    u8 ST;

    u8* m_memory;
};
