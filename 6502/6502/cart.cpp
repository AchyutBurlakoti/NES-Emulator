#include "cart.h"

cart::cart(const std::string& path)
{
	auto ss = std::ostringstream{};
	std::ifstream input_file(path);

	if (!input_file.is_open())
	{
		std::cerr << "error";
		exit(0);
	}

	ss << input_file.rdbuf();

	nes_file = ss.str();

	// check for iNes
	std::string header;

	std::copy_n(nes_file.begin(), 4, std::back_inserter(header));

	if (header != NES_TAG)
	{
		std::cerr << "This is not nes file" << std::endl;
	}

	uint8_t mapper = (nes_file[7] & 0xf0) | (nes_file[6] >> 4);

	uint8_t ines_ver = (nes_file[7] >> 2) & 0x03;

	if (ines_ver != 0)
	{
		std::cerr << "Nes2.0 format is not supported" << std::endl;
	}

	uint8_t mirror = nes_file[6] & 0x01;
	uint8_t four_scr = (nes_file[6] & 0x08) >> 0x03;

	scr_mirroring = four_scr ? (Mirroring)2 : (Mirroring)(mirror & 0x1);

	prg_banks = (uint8_t)nes_file[4];
	chr_banks = (uint8_t)nes_file[5];

	prg_rom_size = prg_banks * PRG_ROM_PAGE_SIZE;
	chr_rom_size = chr_banks * CHR_ROM_PAGE_SIZE;

	prg_rom = (uint8_t*)malloc(prg_rom_size * sizeof(uint8_t));
	chr_rom = (uint8_t*)malloc(chr_rom_size * sizeof(uint8_t));

	uint8_t skip_trainer = (nes_file[6] & 0x04) >> 0x01;

	prg_rom_start = skip_trainer ? 537 : 512;
	chr_rom_start = prg_rom_start + prg_rom_size;
}

void cart::load_prg_chr_rom()
{
	for (int i = 0; i < prg_rom_size; i++)
		prg_rom[i] = nes_file[prg_rom_start + i];
	for (int i = 0; i < chr_rom_size; i++)
		chr_rom[i] = nes_file[chr_rom_start + i];
}

uint16_t cart::prg_rom_len()
{
	return prg_banks * PRG_ROM_PAGE_SIZE;
}
