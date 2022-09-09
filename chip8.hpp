#pragma once
#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using s8 = int8_t;
using f32 = float;

class Chip8
{
public:
	Chip8() = default;

	void loadProgram(const u8* memory) { m_memory = memory; }
	void reset();
	void clock();

	const u8* getScreenMemory() const { return Screen; }

	Chip8(const Chip8&) = delete;
	Chip8& operator=(const Chip8&) = delete;
private:
	union Instruction
	{
		struct
		{
			u8 n1 : 4;
			u8 n2 : 4;
			u8 n3 : 4;
			u8 n4 : 4;
		} nibbles;
		u16 word;
	};

	u8 Screen[256];
	u16 Stack[16];
	u16 I;
	u16 PC : 12;
	u8 GPR[16];
	u8 SP;

	const u8* m_memory;
};
