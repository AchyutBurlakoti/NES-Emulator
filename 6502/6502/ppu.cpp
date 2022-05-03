#include "ppu.h"

ppu::ppu(bus* b)
{
	map = b;

	oam_addr = 0;
	internal_data_buf = 0;

	cycles = 0;
}

void ppu::incr_addr()
{
	uint8_t check = addr.lo++;

	if (check == 0)
	{
		addr.hi++;
	}

	uint16_t total = (uint16_t)addr.hi << 8 | (uint16_t)addr.lo;

	if (total > 0x3fff)
	{
		total = total & 0b11111111111111;

		addr.hi = (uint8_t)(total >> 8);
		addr.lo = (uint8_t)(total & 0xff);
	}
}

void ppu::write_to_ctrl(uint8_t value)
{
	ctrl.reg = value;
}

void ppu::write_to_mask(uint8_t value)
{
	mask.reg = value;
}

void ppu::write_to_oam_addr(uint8_t value)
{
	oam_addr = value;
}

void ppu::write_to_oam_data(uint8_t value)
{
	oam_data[oam_addr] = value;
	oam_addr++;
	
	if (oam_addr == 0)
	{
		std::cerr << "oam_addr is wrapped" << std::endl;
	}
}

void ppu::write_to_scroll(uint8_t value)
{
	if (scroll.latch)
	{
		scroll.scroll_y = value;
	}
	else
	{
		scroll.scroll_x = value;
	}

	scroll.latch = ~scroll.latch;
}

void ppu::write_to_ppu_addr(uint8_t value)
{
	if (addr.hi_ptr)
	{
		addr.hi = value;
	}
	else
	{
		addr.lo = value;
	}

	uint16_t total = (uint16_t)addr.hi << 8 | (uint16_t)addr.lo;

	if (total > 0x3fff)
	{
		total = total & 0b11111111111111;

		addr.hi = (uint8_t)(total >> 8);
		addr.lo = (uint8_t)(total & 0xff);
	}
}

void ppu::write_to_data(uint8_t value)
{
	uint16_t address = (uint16_t)addr.hi << 8 | (uint16_t)addr.lo;

	if (address >= 0x0000 && address <= 0x3eff)
	{
		map->mem_write(value, address, false);
	}
	else if (address == 0x3f10 || address == 0x3f14 || address == 0x3f18 || address == 0x3f1c)
	{
		uint16_t addr_mirror = address - 0x10;
		uint16_t final = (addr_mirror - 0x3f00);
		palette_table[final] = value;
	}
	else if (address >= 0x3f00 && address <= 0x3fff)
	{
		uint16_t final = address - 0x3f00;
		palette_table[final] = value;
	}

	incr_addr();
}

void ppu::write_oam_dma(uint8_t data[256])
{
	for (int i = 0; i < 256; i++)
	{
		oam_data[i] = data[i];
		oam_addr++;
	}
}

uint8_t ppu::read_status()
{
	uint8_t data = status.reg;

	//reset vblank status
	status.reg = status.reg & ~(status.vblk_started);

	// reset latch address
	addr.hi_ptr = true;

	// reset scroll
	scroll.latch = false;

	return data;
}

uint8_t ppu::read_oam_data()
{
	uint8_t temp = oam_data[oam_addr];
	return temp;
}

uint8_t ppu::read_data()
{
	uint16_t address = (uint16_t)addr.hi << 8 | (uint16_t)addr.lo;

	incr_addr();

	uint8_t result;

	if (address >= 0x0000 && address <= 0x3eff)
	{
		result = map->mem_read(address, false);
	}
	else if (address == 0x3f10 || address == 0x3f14 || address == 0x3f18 || address == 0x3f1c)
	{
		uint16_t addr_mirror = address - 0x10;
		uint16_t final = (addr_mirror - 0x3f00);
		result = palette_table[final];
	}
	else if (address >= 0x3f00 && address <= 0x3fff)
	{
		uint16_t final = address - 0x3f00;
		result = palette_table[final];
	}
	else
	{
		std::cerr << "BAD MIRROR ACCES" << std::endl;
	}

	return result;
}

void ppu::connect_bus()
{
}

void ppu::tick(uint8_t cyc)
{
	cycles = cycles + cyc;

	cycle_buf = cyc;
}
