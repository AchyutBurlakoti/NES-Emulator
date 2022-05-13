#include "display.h"

void display::set_pixel(uint16_t x, uint16_t y, type::RGB color)
{
	if (x <= HEIGHT && y <= WIDTH)
	{
		data[x][y] = color;
	}
}
