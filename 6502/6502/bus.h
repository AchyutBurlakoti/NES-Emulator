#pragma once
#include <iostream>
#include "6502.h"
#include "cart.h"



class bus
{
	cart c;
	ppu p;

	uint8_t internal_data_buf;

public:

	void mem_write(uint8_t data, uint16_t address, bool _cpu);
	void mem_write_16(uint16_t data, uint16_t address, bool _cpu);

	uint8_t mem_read(uint16_t address, bool _cpu);
	uint16_t mem_read_16(uint16_t address, bool _cpu);

	uint8_t prg_rom_read(uint16_t address);
	uint16_t mirroring_vram_addr(uint16_t addr);

	void tick(uint8_t cycles);
};
