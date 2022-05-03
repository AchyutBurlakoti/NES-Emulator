#include "6502.h"

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>

int main()
{
	cart* ca;
	cpu* c;
	ppu* p;
	bus* b;

	ca = new cart("mario.nes");
	b = new bus(ca);
	c = new cpu(b);
	p = new ppu(b);

	b->connect_ppu_to_bus(p);

	c->run();
}
