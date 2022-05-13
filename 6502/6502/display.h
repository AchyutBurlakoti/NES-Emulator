#include <iostream>
#include "helpers.h"

class display
{
	static const size_t WIDTH = 256;
	static const size_t HEIGHT = 240;

	type::RGB data[HEIGHT][WIDTH];

public:

	display() = default;

	void set_pixel(uint16_t x, uint16_t y, type::RGB color);

	void draw();
};