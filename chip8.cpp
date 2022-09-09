#include "chip8.hpp"

#include <cassert>

void Chip8::reset()
{
	PC = 0x200;
	SP = 0;
}

void Chip8::clock()
{
	Instruction instruction;
	instruction.word = m_memory[PC++] << 8;
	instruction.word |= m_memory[PC++];

	switch (instruction.nibbles.n4)
	{
	case 0x1:
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
	case 0x6:
		GPR[instruction.nibbles.n3] = instruction.word & 0xFF;
		break;
	case 0x7:
		GPR[instruction.nibbles.n3] += instruction.word & 0xFF;
		break;
	case 0x8:
		switch (instruction.nibbles.n1)
		{
		case 0x2:
			GPR[instruction.nibbles.n3] &= GPR[instruction.nibbles.n2];
			break;
		default:
			assert(false && "Unhandled n1!");
		}
		break;
	case 0xA:
		I = instruction.word & 0x0FFF;
		break;
	case 0xD: {
		u8 x = GPR[instruction.nibbles.n3];
		u8 y = GPR[instruction.nibbles.n2];
		//u8 bytes = instruction.nibbles.n1;
		Screen[y][x / 8] ^= m_memory[I];
	} break;
	default:
		assert(false && "Unhandled n4!");
	}
}
