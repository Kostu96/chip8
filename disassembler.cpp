#include "disassembler.hpp"

#include <iomanip>
#include <iostream>

void disassemble(const u8* program)
{
    for (auto i = 0; i < 400; i += 2)
	{
        std::cout << "$" << std::hex << 0x200 + i << ": ";

        Instruction instruction;
        instruction.word = program[i] << 8;
        instruction.word |= program[i + 1];

        std::cout << "$" << std::hex << std::setw(4) << std::setfill('0') << instruction.word << "  ";

        switch (instruction.nibbles.n4)
        {
        case 0x0:
            if (instruction.word == 0x00E0)
                std::cout << "CLS";
            else if (instruction.word == 0x00EE)
                std::cout << "RET";
            else
                std::cout << "???";
            break;
        case 0x1:
            std::cout << "JMP $" << std::hex << std::setw(3) << std::setfill('0') << (instruction.word & 0xFFF);
            break;
        case 0x2:
            std::cout << "CAL $" << std::hex << std::setw(3) << std::setfill('0') << (instruction.word & 0xFFF);
            break;
        case 0x3:
            std::cout << "SEQ R" << std::hex << (u16)instruction.nibbles.n3 << ", $" << std::hex << std::setw(2) << (instruction.word & 0xFF);
            break;
        case 0x4:
            std::cout << "SNE R" << std::hex << (u16)instruction.nibbles.n3 << ", $" << std::hex << std::setw(2) << (instruction.word & 0xFF);
            break;
        case 0x5:
            std::cout << "SEQ R" << std::hex << (u16)instruction.nibbles.n3 << ", R" << std::hex << (u16)instruction.nibbles.n2;
            break;
        case 0x6:
            std::cout << "MOV R" << std::hex << (u16)instruction.nibbles.n3 << ", $" << std::hex << std::setw(2) << (instruction.word & 0xFF);
            break;
        case 0x7:
            std::cout << "ADD R" << std::hex << (u16)instruction.nibbles.n3 << ", $" << std::hex << std::setw(2) << (instruction.word & 0xFF);
            break;
        case 0x8:
            std::cout << "??? ALU";
            /*switch (instruction.nibbles.n1)
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
            }*/
            break;
        case 0x9:
            std::cout << "SNE R" << std::hex << (u16)instruction.nibbles.n3 << ", R" << std::hex << (u16)instruction.nibbles.n2;
            break;
        case 0xA:
            std::cout << "MOV I, $" << std::hex << std::setw(3) << (instruction.word & 0xFFF);
            break;
        case 0xC:
            std::cout << "MOV R" << std::hex << (u16)instruction.nibbles.n3 << ", $" << std::hex << std::setw(2) << (instruction.word & 0xFF);
            break;
        case 0xD: {
            std::cout << "DSP R" << std::hex << (u16)instruction.nibbles.n3 << ", R" << std::hex << (u16)instruction.nibbles.n2 << ", $" << std::hex << (u16)instruction.nibbles.n1;
        } break;
        case 0xE:
            if ((instruction.word & 0xFF) == 0x9E)
                std::cout << "SKE ?";
            else if ((instruction.word & 0xFF) == 0xA1)
                std::cout << "SNK ?";
            break;
        case 0xF:
            switch (instruction.word & 0xFF)
            {
            case 0x07:
                std::cout << "MOV R" << std::hex << (u16)instruction.nibbles.n3 << ", DT";
                break;
            case 0x0A:
                std::cout << "???";
                break;
            case 0x15:
                std::cout << "MOV DT, R" << std::hex << (u16)instruction.nibbles.n3;
                break;
            case 0x18:
                std::cout << "MOV ST, R" << std::hex << (u16)instruction.nibbles.n3;
                break;
            case 0x1E:
                std::cout << "ADD I, R" << std::hex << (u16)instruction.nibbles.n3;
                break;
            case 0x29:
                std::cout << "CHR R" << std::hex << (u16)instruction.nibbles.n3;
                break;
            case 0x33:
                std::cout << "BCD R" << std::hex << (u16)instruction.nibbles.n3; 
                break;
            case 0x55:
                /*for (u8 i = 0; i <= instruction.nibbles.n3; i++)
                    m_memory[I++] = GPR[i];*/
                break;
            case 0x65:
                /*for (u8 i = 0; i <= instruction.nibbles.n3; i++)
                    GPR[i] = m_memory[I++];*/
                break;
            }
            break;
        default:
            std::cout << "???";
        }

        std::cout << '\n';
	}
}
