#pragma once
#include <iostream>
// make array size addressable it has been changed for ppu
constexpr auto MEMORY_SIZE = 0x0ffff;
uint8_t memory[MEMORY_SIZE];