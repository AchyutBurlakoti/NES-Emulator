#pragma once
#include <iostream>
#include <vector>

#include "bus.h"

struct addr_reg
{
	uint8_t hi = 0;
	uint8_t lo = 0;

	bool hi_ptr = true;
};

struct ctrl_reg
{
	uint8_t reg;

	uint8_t nametable1    = 0b00000001;
	uint8_t nametable2    = 0b00000010;
	uint8_t vram_addr_inc = 0b00000100;
	uint8_t spt_addr      = 0b00001000;
	uint8_t bpt_addr      = 0b00010000;
	uint8_t sprite_size   = 0b00100000;
	uint8_t ppu_ms_sel    = 0b01000000;
	uint8_t nmi           = 0b10000000;
};

struct mask_reg
{
	uint8_t reg;

	uint8_t greyscale     = 0b00000001;
	uint8_t show_bg_lm8px = 0b00000010;
	uint8_t show_sp_lm8px = 0b00000100;
	uint8_t show_bg       = 0b00001000;
	uint8_t show_sp       = 0b00010000;
	uint8_t em_red        = 0b00100000;
	uint8_t em_green      = 0b01000000;
	uint8_t em_blue       = 0b10000000;
};

struct stat_reg
{
	uint8_t reg;

	uint8_t sp_ov         = 0b00100000;
	uint8_t sp0_hit       = 0b01000000;
	uint8_t vblk_started  = 0b10000000;
};

struct scrl_reg
{
	uint8_t scroll_x = 0;
	uint8_t scroll_y = 0;

	bool latch = false;
};

class ppu
{

public:

	uint8_t* chr_rom;

	Mirroring mirror;

	ctrl_reg ctrl;
	mask_reg mask;
	stat_reg status;
	scrl_reg scroll;
	addr_reg addr;

	uint8_t oam_addr;
	std::vector<uint8_t> oam_data;
	uint8_t palette_table[32] = { 0 };
	uint8_t vram[2048] = { 0 };

	uint8_t internal_data_buf;

	bus* map;

	int cycles;
	uint8_t cycle_buf;
	uint16_t scanline = 0;

public:

	ppu() = default;
	ppu(bus* b);
	void incr_addr();

	void write_to_ctrl(uint8_t  value);
	void write_to_mask(uint8_t value);
	void write_to_oam_addr(uint8_t value);
	void write_to_oam_data(uint8_t value);
	void write_to_scroll(uint8_t value);
	void write_to_ppu_addr(uint8_t value);
	void write_to_data(uint8_t value);
	void write_oam_dma(uint8_t data[256]);

	uint8_t read_status();
	uint8_t read_oam_data();
	uint8_t read_data();

	void connect_bus();

	bool tick(uint8_t cyc);

	bool nmi_interrupt_stats = false;
};