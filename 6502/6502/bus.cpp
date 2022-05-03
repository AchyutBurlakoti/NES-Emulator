#include "ppu.h"
#include "ram.h"

void bus::mem_write(uint8_t data, uint16_t address, bool _cpu)
{
	if (_cpu)
	{
		if (address >= 0x0000 && address <= 0x1fff)
		{
			uint16_t mirror_addr    = address & 0b0000011111111111;
			cpu_vram[mirror_addr]   = data;
		}

		else if (address == 0x2000)
		{
			p->write_to_ctrl(data);
		}

		else if (address == 0x2006)
		{
			p->write_to_ppu_addr(data);
		}

		else if (address == 0x2007)
		{
			p->write_to_data(data);
		}

		else if (address >= 0x2008 && address <= 0x3fff)
		{
			uint16_t mirror_addr = address & 0b0010000000000111;

			mem_write(data, mirror_addr, _cpu);
		}

		else if (address >= 0x8000 && address <= 0xffff)
		{
			std::cerr << "can't write prg-rom space" << std::endl;
		}

	}

	else
	{

		if (address >= 0x0000 && address <= 0x1fff)
		{
			std::cerr << "can't write to chr-rom space" << std::endl;
		}

		else if (address >= 0x2000 && address <= 0x2fff)
		{
			uint16_t mirror_addr    = mirroring_vram_addr(address);
			ppu_vram[mirror_addr]   = data;
		}

		else if (address >= 0x3000 && address <= 0x3eff)
		{
			// Mirror 2000 - 2eff
			std::cerr << "danger zone" << std::endl;
		}
	}
}

void bus::mem_write_16(uint16_t data, uint16_t address, bool _cpu)
{
	auto lower_order_byte = data & 0xff;
	mem_write(lower_order_byte, address, _cpu);

	auto higher_order_byte = (data & 0xff0) > 8;
	mem_write(higher_order_byte, address + 1, _cpu);
}

uint8_t bus::mem_read(uint16_t address, bool _cpu)
{
	if (_cpu)
	{

		if (address >= 0x0000 && address <= 0x1fff)
		{
			uint16_t mirror_addr = address & 0b0000011111111111;
			return cpu_vram[mirror_addr];
		}

		else if (address == 0x2000 || address == 0x2001 || address == 0x2003 || address == 0x2005 || address == 0x2006 || address == 0x4014)
		{
			std::cerr << "write only address of ppu" << std::endl;
		}

		else if (address == 0x2007)
		{
			p->read_data();
		}

		else if (address >= 0x2008 && address <= 0x3fff)
		{
			uint16_t mirror_addr = address & 0b0010000000000111;
			return mem_read(mirror_addr, _cpu);
		}

		else if (address >= 0x8000 && address <= 0xffff)
		{
			return prg_rom_read(address);
		}

		else
		{
			std::cerr << "error in cpu addressing" << std::endl;
		}
	}
	else
	{
		uint8_t result;

		if (address >= 0x0000 && address <= 0x1fff)
		{
			result              = internal_data_buf;
			internal_data_buf   = c->chr_rom[address];
		}

		else if (address >= 0x2000 && address <= 0x2fff)
		{
			result                 = internal_data_buf;
			uint16_t mirror_addr   = mirroring_vram_addr(address);
			internal_data_buf      = ppu_vram[mirror_addr];
		}

		else if (address >= 0x3000 && address <= 0x3eff)
		{
			// Mirror 2000 - 2eff
			std::cerr << "danger zone" << std::endl;
		}

		return result;
	}
}

uint16_t bus::mem_read_16(uint16_t address, bool _cpu)
{
	uint16_t lower_order_byte     = mem_read(address, _cpu);
	uint16_t higher_order_bypte   = mem_read(address + 1, _cpu);

	return higher_order_bypte << 8 | lower_order_byte;
}

uint8_t bus::prg_rom_read(uint16_t address)
{
	address -= 0x8000;

	if (c->prg_rom_len() == 0x4000 && address >= 0x4000)
	{
		address = address % 0x4000;
	}

	return c->prg_rom[address];
}

void bus::tick(uint8_t cyc)
{
	p->tick(cyc * 3);
}

void bus::connect_ppu_to_bus(ppu* pp)
{
	p = pp;
}

bus::bus(cart* ca)
{
	c = ca;
}

uint16_t bus::mirroring_vram_addr(uint16_t addr)
{
	uint16_t mirrored_vram   = addr & 0b10111111111111;
	uint16_t vram_index      = mirrored_vram - 0x2000;
	uint16_t name_table      = vram_index / 0x400;

	switch (c->scr_mirroring)
	{
		case Mirroring::VERTICAL:

			if (name_table == 2 | name_table == 3)
			{
				vram_index = vram_index - 0x800;
			}

		case Mirroring::HORIZONTAL:

			if (name_table == 2 | name_table == 1)
			{
				vram_index = vram_index - 0x400;
			}
			else
			{
				vram_index = vram_index - 0x800;
			}
	}
	return vram_index;
}
