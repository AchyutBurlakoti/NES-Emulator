#include <stdlib.h>

#include "renderer.h"

uint8_t* background_palette(ppu* p, uint16_t tile_col, uint16_t tile_row)
{
	uint8_t* color = new uint8_t[4];

	// comment
	int atr_tbl_idx = tile_row / 4 * 8 + tile_col / 4;
	int value = 0x3c0 + atr_tbl_idx;
	int attr_byte = p->vram[value];

	int palete_idx;

	int a = tile_col % 4 / 2;
	int b = tile_row % 4 / 2;

	if (a == 0 && b == 0)
	{
		palete_idx = attr_byte & 0b11;
	}
	else if (a == 1 && b == 0)
	{
		palete_idx = (attr_byte >> 2) & 0b11;
	}
	else if (a == 0 && b == 1)
	{
		palete_idx = (attr_byte >> 4) & 0b11;
	}
	else if (a == 1 && b == 1)
	{
		palete_idx = (attr_byte >> 6) & 0b11;
	}
	else
	{

	}

	int palette_start = 1 + palete_idx * 4;

	color[0] = p->palette_table[0];
	color[1] = p->palette_table[palette_start];
	color[2] = p->palette_table[palette_start + 1];
	color[3] = p->palette_table[palette_start + 2];

	return color;
}

uint8_t* sprite_palette(ppu* p, uint8_t palette_idx)
{
	int start = 0x11 + (palette_idx * 4);

	uint8_t* data = (uint8_t*) malloc(4 * sizeof(uint8_t));

	data[0] = 0;
	data[1] = p->palette_table[start];
	data[2] = p->palette_table[start + 1];
	data[3] = p->palette_table[start + 2];

	return data;
}

void render(ppu* p, display* d)
{
	uint16_t bank = (p->ctrl.bpt_addr & p->ctrl.reg) ? 0x1000 : 0;

	for (int i = 0; i < 0x3c0; i++)
	{
		uint16_t tile = p->vram[i];
		uint16_t tile_col = i % 32;
		uint16_t tile_row = i / 32;

		uint8_t* tiles = new uint8_t[16];

		uint8_t* palette = background_palette(p, tile_col, tile_row);

		for (int j = 0; j < 16; j++)
		{
			int z = (bank + tile * 16) + j;
			tiles[j] = p->chr_rom[z];
		}

		for (int y = 0; y < 8; y++)
		{
			int upper = tiles[y];
			int lower = tiles[y + 8];

			for (int x = 7; x > -1; x--)
			{
				int value = (1 & lower) << 1 | (1 & upper);
				upper = upper >> 1;
				lower = lower >> 1;

				type::RGB color;

				switch (value)
				{
				case 0: color = SYSTEM_PALETTE[p->palette_table[0]];
				case 1: color = SYSTEM_PALETTE[palette[1]];
				case 2: color = SYSTEM_PALETTE[palette[2]];
				case 3: color = SYSTEM_PALETTE[palette[3]];
				default: std::cerr << "not expected" << std::endl;
				}

				d->set_pixel(tile_col * 8 + x, tile_row * 8 + y, color);
			}
		}
	}

	for (int i = p->oam_data.size(); i > -1; i -= 4)
	{
		uint16_t tile_idx = p->oam_data[i + 1];
		uint16_t tile_x = p->oam_data[i + 3];
		uint16_t tile_y = p->oam_data[i];

		bool flip_vertical, flip_horizontal;

		if ((p->oam_data[i + 2] >> 7 & 1) == 1)
		{
			flip_vertical = true;
		}
		else
		{
			flip_vertical = false;
		}

		if ((p->oam_data[i + 2] >> 6 & 1) == 1)
		{
			flip_horizontal = true;
		}
		else
		{
			flip_horizontal = false;
		}

		int palette_idx = p->oam_data[i + 2] & 0b11;
		uint8_t* palette = sprite_palette(p, palette_idx);

		int bank = (p->ctrl.reg & p->ctrl.spt_addr) ? 0x1000 : 0;

		uint8_t* tiles = new uint8_t[16];
		for (int j = 0; j < 16; j++)

		{
			int z = (bank + tile_idx * 16) + j;
			tiles[j] = p->chr_rom[z];
		}

		for (int y = 0; y < 8; y++)
		{
			int upper = tiles[y];
			int lower = tiles[y + 8];

			for (int x = 7; x > -1; x--)
			{
				int value = (1 & lower) << 1 | (1 & upper);
				upper = upper >> 1;
				lower = lower >> 1;

				bool setpixel = false;

				type::RGB color;

				switch (value)
				{
				case 0: setpixel = false;
				case 1: {
					color = SYSTEM_PALETTE[palette[1]];
					setpixel = true;
				}
				case 2: {
					color = SYSTEM_PALETTE[palette[2]];
					setpixel = true;
				}
				case 3: {
					color = SYSTEM_PALETTE[palette[3]];
					setpixel = true;
				}
				default: std::cerr << "not expected" << std::endl;
				}

				if (setpixel)
				{
					if (flip_horizontal == false && flip_vertical == false)
					{
						d->set_pixel(tile_x + x, tile_y + y, color);
					}
					else if (flip_horizontal == true && flip_vertical == false)
					{
						d->set_pixel(tile_x + 7 - x, tile_y + y, color);
					}
					else if (flip_horizontal == false && flip_vertical == true)
					{
						d->set_pixel(tile_x + x, tile_y + 7 - y, color);
					}
					else if (flip_horizontal == true && flip_vertical == true)
					{
						d->set_pixel(tile_x + 7 - x, tile_y + 7 - y, color);
					}
				}
			}
		}
	}
}
