#include "bus.h"
#include "ram.h"

#define MEMORY_CHECK(x) if(x >= 0xffff) std::cout << "MEMORY OUT OF SIZE!!!!"

void mapper::mem_write(uint8_t data, uint16_t address)
{
	MEMORY_CHECK(address);

	memory[address] = data;

	// if cpu
	// if cpu ram  0x0000 - 0x07ff 2 kb
	// memory[address & 0x7ff] = data;
}

void mapper::mem_write_16(uint16_t data, uint16_t address)
{
	MEMORY_CHECK(address);

	auto lower_order_byte = data & 0xff;
	memory[address] = lower_order_byte;

	auto higher_order_byte = (data & 0xff0) > 8;
	memory[address + 1] = higher_order_byte;
}

uint8_t mapper::mem_read(uint16_t address)
{
	MEMORY_CHECK(address);
	return (address >= 0x8000 && address <= 0xFFFF) ? prg_rom_read(address) : memory[address];
}

uint16_t mapper::mem_read_16(uint16_t address)
{
	MEMORY_CHECK(address);

	uint16_t lower_order_byte = memory[address];
	uint16_t higher_order_bypte = memory[address + 1];
	return higher_order_bypte << 8 | lower_order_byte;
}

uint8_t mapper::prg_rom_read(uint16_t address)
{
	address -= 0x8000;

	if (c.prg_rom_len() == 0x4000 && address >= 0x4000)
	{
		address = address % 0x4000;
	}

	return c.prg_rom[address];
}
