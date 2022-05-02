#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <iterator>

#define PRG_ROM_PAGE_SIZE 16384
#define CHR_ROM_PAGE_SIZE 8192

const std::string NES_TAG = { 0x4E, 0x45, 0x53, 0x1A };

enum Mirroring
{
	HORIZONTAL,
	VERTICAL,
	FOUR_SCREEN,
};

class cart
{
public :
	uint8_t* prg_rom;
	uint8_t* chr_rom;

	uint8_t prg_banks;
	uint8_t chr_banks;

	Mirroring scr_mirroring;

private:

	uint8_t mapper;

	std::string nes_file;

	uint16_t prg_rom_start;
	uint16_t chr_rom_start;

	uint16_t prg_rom_size;
	uint16_t chr_rom_size;

public :

	cart(const std::string& path);
	void load_prg_chr_rom();
	uint16_t prg_rom_len();
};