#include "chip8.hpp"

#include <cassert>
#include <random>

void Chip8::reset()
{
    std::srand(1234567890);

    PC = 0x200;
    SP = 0;

    DT = ST = 0;
}

void Chip8::clock()
{
    if (DT) DT--;
    if (ST) ST--;

    Instruction instruction;
    instruction.word = m_memory[PC++] << 8;
    instruction.word |= m_memory[PC++];

    switch (instruction.nibbles.n4)
    {
    case 0x0:
        if (instruction.word == 0x00E0)
            std::memset(Screen, 0, 64 * 32);
        else if (instruction.word == 0x00EE)
            PC = Stack[--SP];
        else
            assert(false);
        break;
    case 0x1:
        PC = instruction.word & 0x0FFF;
        break;
    case 0x2:
        Stack[SP++] = PC;
        PC = instruction.word & 0x0FFF;
        break;
    case 0x3:
        if (GPR[instruction.nibbles.n3] == (instruction.word & 0xFF))
            PC += 2;
        break;
    case 0x4:
        if (GPR[instruction.nibbles.n3] != (instruction.word & 0xFF))
            PC += 2;
        break;
    case 0x5:
        if (GPR[instruction.nibbles.n3] == GPR[instruction.nibbles.n2])
            PC += 2;
        break;
    case 0x6:
        GPR[instruction.nibbles.n3] = instruction.word & 0xFF;
        break;
    case 0x7:
        GPR[instruction.nibbles.n3] += instruction.word & 0xFF;
        break;
    case 0x8:
        switch (instruction.nibbles.n1)
        {
        case 0x0:
            GPR[instruction.nibbles.n3] = GPR[instruction.nibbles.n2];
            break;
        case 0x1:
            GPR[instruction.nibbles.n3] |= GPR[instruction.nibbles.n2];
            break;
        case 0x2:
            GPR[instruction.nibbles.n3] &= GPR[instruction.nibbles.n2];
            break;
        case 0x3:
            GPR[instruction.nibbles.n3] ^= GPR[instruction.nibbles.n2];
            break;
        case 0x4: {
            u16 temp = (u16)GPR[instruction.nibbles.n3] + (u16)GPR[instruction.nibbles.n2];
            GPR[0xF] = temp >> 8;
            GPR[instruction.nibbles.n3] = temp & 0xFF;
        } break;
        case 0x5: {
            s16 temp = (s16)GPR[instruction.nibbles.n3] - (s16)GPR[instruction.nibbles.n2];
            GPR[0xF] = temp < 0 ? 0 : 1;
            GPR[instruction.nibbles.n3] = temp & 0xFF;
        } break;
        case 0x6:
            GPR[0xF] = GPR[instruction.nibbles.n3] & 1;
            GPR[instruction.nibbles.n3] >>= 1;
            break;
        case 0xE:
            GPR[0xF] = GPR[instruction.nibbles.n3] >> 7;
            GPR[instruction.nibbles.n3] <<= 1;
            break;
        default:
            assert(false && "Unhandled n1!");
        }
        break;
    case 0x9:
        if (GPR[instruction.nibbles.n3] != GPR[instruction.nibbles.n2])
            PC += 2;
        break;
    case 0xA:
        I = instruction.word & 0x0FFF;
        break;
    case 0xC:
        GPR[instruction.nibbles.n3] = (std::rand() % 256) & (instruction.word & 0xFF);
        break;
    case 0xD: {
        u8 x = GPR[instruction.nibbles.n3];
        u8 y = GPR[instruction.nibbles.n2];
        u8 bytes = instruction.nibbles.n1;
        GPR[0xF] = 0;
        for (u8 i = 0; i < bytes; i++)
        {
            for (u8 bit = 0; bit < 8; ++bit)
            {
                u16 index = (y + i) * 64 + x + bit;
                if (Screen[index] && (m_memory[I] >> (7 - bit)) & 1) GPR[0xF] = 1;
                Screen[index] ^= (m_memory[I] >> (7 - bit)) & 1;
            }
        }
    } break;
    case 0xE:
        if ((instruction.word & 0xFF) == 0x9E)
            ; // temp
        else if ((instruction.word & 0xFF) == 0xA1)
            PC += 2; // temp
        break;
    case 0xF:
        switch (instruction.word & 0xFF)
        {
        case 0x07:
            GPR[instruction.nibbles.n3] = DT;
            break;
        case 0x0A:
            while (true);
            break;
        case 0x15:
            DT = GPR[instruction.nibbles.n3];
            break;
        case 0x18:
            ST = GPR[instruction.nibbles.n3];
            break;
        case 0x1E:
            I += GPR[instruction.nibbles.n3];
            break;
        case 0x29:
            I = GPR[instruction.nibbles.n3] * 5;
            break;
        case 0x33: {
            u8 n1 = GPR[instruction.nibbles.n3] % 10;
            u8 n2 = (GPR[instruction.nibbles.n3] / 10) % 10;
            u8 n3 = GPR[instruction.nibbles.n3] / 100;
            m_memory[I] = n3;
            m_memory[I + 1] = n2;
            m_memory[I + 2] = n1;
        } break;
        case 0x55:
            for (u8 i = 0; i <= instruction.nibbles.n3; i++)
                m_memory[I++] = GPR[i];
            break;
        case 0x65:
            for (u8 i = 0; i <= instruction.nibbles.n3; i++)
                GPR[i] = m_memory[I++];
            break;
        default:
            assert(false);
        }
        break;
    default:
        assert(false && "Unhandled n4!");
    }
}
