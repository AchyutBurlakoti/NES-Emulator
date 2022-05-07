#pragma once
#include <iostream>

constexpr auto CPU_MEMORY_SIZE = 0x7ff;
uint8_t cpu_vram[CPU_MEMORY_SIZE];

constexpr auto PPU_MEMORY_SIZE = 0xfff;
uint8_t ppu_vram[PPU_MEMORY_SIZE];