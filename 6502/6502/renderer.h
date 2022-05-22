#pragma once
#include <iostream>
#include "display.h"
#include "ppu.h"

uint8_t* background_pallette(ppu* p, uint16_t tile_col, uint16_t tile_row);

uint8_t* sprite_palette(ppu* p, uint8_t plt_inx);

void render(ppu* p, display* d);