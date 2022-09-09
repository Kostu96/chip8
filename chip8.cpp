#include "chip8.hpp"

#include <cassert>
#include <random>

void Chip8::reset()
{
	std::srand(1234567890);

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
	case 0x0:
		if (instruction.word == 0x00EE)
		{
			PC = Stack[--SP];
		}
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
		for (u8 i = 0; i < bytes; ++i)
		{
			u8 index = y * 8 + ((x + i) / 8);
			Screen[index] ^= m_memory[I];
		}
	} break;
	case 0xF:
		while (true);
		break;
	default:
		assert(false && "Unhandled n4!");
	}
}
