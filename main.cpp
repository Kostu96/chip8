#include "chip8.hpp"

#include <fstream>
#include <iostream>

int main()
{
	u8 program[0x1000];

	std::ifstream fin{ "roms/Breakout [Carmelo Cortez, 1979].ch8", std::ios::binary };
	fin.read((char*)(program + 0x200), 0xE00);
	fin.close();
	
	Chip8 chip8{};
	chip8.loadProgram(program);
	chip8.reset();
	while (true)
	{
		system("cls");
		chip8.clock();

		for (u8 row = 0; row < 32; row++)
		{
			for (u8 byte = 0; byte < 8; byte++)
			{
				u8 x = chip8.Screen[row][byte];
				for (int bit = 7; bit >= 0; bit--)
					std::cout << (((x >> bit) & 1) ? 'x' : ' ');
			}
			std::cout << '\n';
		}
	}

	return 0;
}
