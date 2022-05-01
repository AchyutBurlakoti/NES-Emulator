#pragma once
#include <iostream>
#include "6502.h"
#include "cart.h"

class mapper
{
	cart c;
public:
	void mem_write(uint8_t data, uint16_t address);
	void mem_write_16(uint16_t data, uint16_t address);
	uint8_t mem_read(uint16_t address);
	uint16_t mem_read_16(uint16_t address);
	uint8_t prg_rom_read(uint16_t address);
};
