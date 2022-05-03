#include "6502.h"

void cpu::print_contents()
{
	printf("\nAccumulator : %x   X : %x   Y : %x   stack pointer : %x    program counter : %x  flags register : %x", accumulator, x, y, stack_pointer, program_counter, processor_status);
}

uint8_t cpu::cycle(addressing_mode mode, bool simple)
{
	if (mode == addressing_mode::accumulator)
	{
		return 2;
	}

	if (simple)
	{
		if (mode == addressing_mode::zero_page)
		{
			return 3;
		}

		else if (mode == addressing_mode::zero_page_x || mode == addressing_mode::zero_page_y
			|| mode == addressing_mode::absolute || mode == addressing_mode::absolute_x
			|| mode == addressing_mode::absolute_y)
		{
			return 4;
		}

		else if (mode == addressing_mode::indirect_x)
		{
			return 6;
		}

		else if (mode == addressing_mode::indirect_y)
		{
			return 5;
		}
	}

	else
	{
		if (mode == addressing_mode::immediate)
		{
			return 2;
		}

		else if (mode == addressing_mode::zero_page)
		{
			return 5;
		}

		else if (mode == addressing_mode::zero_page_x || mode == addressing_mode::absolute)
		{
			return 6;
		}

		else if (mode == addressing_mode::absolute_x)
		{
			return 7;
		}
	}
	return uint8_t(-1);
}

cpu::cpu(bus* b) :
	modes{ addressing_mode::immediate,
	addressing_mode::zero_page,
	addressing_mode::zero_page_x,
	addressing_mode::zero_page_y,
	addressing_mode::absolute,
	addressing_mode::absolute_x,
	addressing_mode::absolute_y,
	addressing_mode::indirect_x,
	addressing_mode::indirect_y,
	addressing_mode::accumulator,
	addressing_mode::no_address
},
functions{ &cpu::lda, &cpu::adc, &cpu::bitwise_and, &cpu::asl, &cpu::bit, &cpu::cmp, &cpu::cpx, &cpu::cpy, 
	&cpu::dec, &cpu::eor, &cpu::inc, &cpu::ldx, &cpu::ldy, &cpu::lsr, &cpu::nop, &cpu::ora, &cpu::rol, &cpu::ror,
	&cpu::sbc, &cpu::bcc, &cpu::bcs, &cpu::beq, &cpu::bmi, &cpu::bne, &cpu::bpl, &cpu::bvc, &cpu::bvs,
	&cpu::rti, &cpu::rts, &cpu::brk, 
	&cpu::clc, &cpu::sec, &cpu::cli, &cpu::sei, &cpu::clv, &cpu::cld, &cpu::sed, 
	&cpu::jmp, &cpu::jsr, 
	&cpu::txs, &cpu::tsx, &cpu::pha, &cpu::pla, &cpu::php, &cpu::plp,
	&cpu::tax, &cpu::txa, &cpu::dex, &cpu::inx, &cpu::tay, &cpu::tya, &cpu::dey, &cpu::iny,
	&cpu::sta, &cpu::stx, &cpu::sty
},
addr_mode{ &cpu::immediate, &cpu::zero_page, &cpu::zero_page_x, &cpu::zero_page_y, &cpu::absolute, &cpu::absolute_x,
	&cpu::absolute_y, &cpu::indirect_x, &cpu::indirect_y
}
{
	accumulator = 0x00;
	x = 0x00;
	y = 0x00;
	stack_pointer = 0xff;
	processor_status = 0x00;
	program_counter = 0x0000;
	cycles = 0;

	std::cout << "Initializing the CPU......\n";

	opcodes_addressing_mode_mapping = {
		// for immediate
		{0x69, 0}, {0x29, 0}, {0xc9 , 0}, {0xe0, 0}, {0xc0, 0}, {0x49, 0}, {0xa9 , 0}, {0xa2, 0}, {0xa0, 0}, {0x09, 0}, {0xe9, 0},
		//for zero page
		{0x65, 1}, {0x25, 1}, {0x06, 1}, {0x24, 1}, {0xc5, 1}, {0xe4, 1}, {0xc4, 1}, {0xc6, 1}, {0x45, 1}, {0xe6, 1}, {0xa5, 1}, {0xa6, 1}, {0xa4, 1}, {0x46, 1}, {0x05, 1}, {0x26, 1}, {0x66, 1}, {0xe5, 1}, {0x85, 1}, {0x86, 1}, {0x84, 1},
		// for zero page X
		{0x75, 2}, {0x35, 2}, {0x16, 2}, {0xd5, 2}, {0xd6, 2}, {0x55, 2}, {0x55, 2}, {0xf6, 2}, {0xb5, 2}, {0xb4, 2}, {0x56, 2}, {0x15, 2}, {0x36, 2}, {0x76, 2}, {0xf5, 2}, {0x95, 2}, {0x94, 2},
		// for zero page y
		{0xb6, 3}, {0xac, 3}, {0x96, 3},
		// for absolute 
		{0x6d, 4}, {0x2d, 4}, {0x0e, 4}, {0xcd, 4}, {0xec, 4}, {0xcc, 4}, {0xce, 4}, {0x4d, 4}, {0xee, 4}, {0x4c, 4}, {0x20, 4}, {0xad, 4}, {0xae, 4}, {0xbc, 4}, {0x4e, 4}, {0x0d, 4}, {0x2e, 4}, {0x6e, 4}, {0xed, 4}, {0x8d, 4}, {0x8e, 4}, {0x8c, 4},
		// for absolute x
		{0x7d, 5}, {0x3d, 5}, {0x1e, 5}, {0x2c, 5}, {0xdd, 5}, {0xde, 5}, {0x5d, 5}, {0xfe, 5}, {0x1d, 5}, {0x3e, 5}, {0x7e, 5}, {0xfd, 5}, {0x9d, 5},
		// for absolute y 
		{0x79, 6}, {0x39, 6}, {0xd9, 6}, {0x59, 6}, {0xb9, 6}, {0xbe, 6}, {0x19, 6}, {0xf9, 6}, {0x99, 6},
		// for indirect x
		{0x61, 7}, {0x21, 7}, {0xc1, 7}, {0x41, 7}, {0xa1, 7}, {0x01, 7}, {0xe1, 7}, {0x81, 7},
		// for indirect y
		{0x71, 8}, {0x31, 8}, {0xd1, 8}, {0x51, 8}, {0xb1, 8}, {0x11, 8}, {0xf1, 8}, {0x91, 8},
		// for accumulator mode
		{0x0a, 9}, {0x4a, 9}, {0x2a, 9},
		// for no address mode
		{0x10, 10}, {0x30, 10}, {0x50, 10}, {0x70, 10}, {0x90, 10}, {0xb0, 10}, {0xd0, 10}, {0xf0, 10}, {0x00, 10}, {0x18, 10}, {0x38, 10}, {0x58, 10}, {0x78, 10}, {0xb8, 10}, {0xd8, 10}, {0xf8, 10}, {0xea, 10},
		{0xaa, 10}, {0x8a, 10}, {0xca, 10}, {0xe8, 10}, {0xa8, 10}, {0x98, 10}, {0x88, 10}, {0xc8, 10},
		{0x40, 10}, {0x60, 10}, {0x9a, 10}, {0xba, 10}, {0x48, 10}, {0x68, 10}, {0x08, 10}, {0x28, 10}
	};
	opcodes_func_mapping = {
		// lda
		{0xa9, LDA}, {0xa5, LDA}, {0xb5, LDA}, {0xad, LDA}, {0xbd, LDA}, {0xb9, LDA}, {0xa1, LDA}, {0xb1, LDA}, 
		// adc
		{0x69, ADC}, {0x65, ADC}, {0x75, ADC}, {0x6d, ADC}, {0x7d, ADC}, {0x79, ADC}, {0x61, ADC}, {0x71, ADC},
		// and
		{0x29, AND}, {0x25, AND}, {0x35, AND}, {0x2d, AND}, {0x3d, AND}, {0x39, AND}, {0x21, AND}, {0x31, AND},
		// asl
		{0x0a, ASL}, {0x06, ASL}, {0x16, ASL}, {0x0e, ASL}, {0x1e, ASL},
		// bit
		{0x24, BIT}, {0x2c, BIT}, 
		// cmp
		{0xc9, CMP}, {0xc5, CMP}, {0xd5, CMP}, {0xcd, CMP}, {0xdd, CMP}, {0xd9, CMP}, {0xc1, CMP}, {0xd1, CMP},
		// cpx
		{0xe0, CPX}, {0xe4, CPX}, {0xec, CPX},
		// cpy
		{0xc0, CPY}, {0xc4, CPY}, {0xcc, CPY},
		// dec 
		{0xc6, DEC}, {0xd6, DEC}, {0xce, DEC}, {0xde, DEC},
		// eor
		{0x49, EOR}, {0x45, EOR}, {0x55, EOR}, {0x4d, EOR}, {0x5d, EOR}, {0x59, EOR}, {0x41, EOR}, {0x51, EOR},
		// inc
		{0xe6, INC}, {0xf6, INC}, {0xee, INC}, {0xfe, INC},
		// ldx
		{0xa2, LDX}, {0xa6, LDX}, {0xb6, LDX}, {0xae, LDX}, {0xbe, LDX},
		// ldy
		{0xa0, LDY}, {0xa4, LDY}, {0xb4, LDY}, {0xac, LDY}, {0xbc, LDY},
		// lsr
		{0x4a, LSR}, {0x46, LSR}, {0x56, LSR}, {0x4e, LSR}, {0x5e, LSR},
		// nop
		{0xea, NOP},
		// ora
		{0x09, ORA}, {0x05, ORA}, {0x15, ORA}, {0x0d, ORA}, {0x1d, ORA}, {0x19, ORA}, {0x01, ORA}, {0x11, ORA},
		// rol
		{0x2a, ROL}, {0x26, ROL}, {0x36, ROL}, {0x2e, ROL}, {0x3e, ROL},
		// sbc
		{0xe9, SBC}, {0xe5, SBC}, {0xf5, SBC}, {0xed, SBC}, {0xfd, SBC}, {0xf9, SBC}, {0xe1, SBC}, {0xf1, SBC},
		// branching
		{0x90, BCC}, {0xb0, BCS}, {0xf0, BEQ}, {0x30, BMI}, {0xd0, BNE}, {0x10, BPL}, {0x50, BVC}, {0x70, BVS},
		// interrupt
		{0x40, RTI}, {0x60, RTS}, {0x00, BRK}, 
		// set clear 
		{0x18, CLC}, {0x30, SEC}, {0x58, CLI}, {0x78, SEI}, {0xb8, CLV}, {0xd8, CLD}, {0xf8, SED},
		// branch
		{0x4c, JMP}, {0x6c, JMP}, {0x20, JSR},
		// stack instruction
		{0x9a, TXS}, {0xba, TSX}, {0x48, PHA}, {0x68, PLA}, {0x08, PHP}, {0x28, PLP},
		// resgister instruction
		{0xaa, TAX}, {0x8a, TXA}, {0xca, DEX}, {0xe8, INX}, {0xa8, TAY}, {0x98, TYA}, {0x88, DEY}, {0xc8, INY},
		// sta
		{0x85, STA}, {0x95, STA}, {0x8d, STA}, {0x9d, STA}, {0x99, STA}, {0x81, STA}, {0x91, STA},
		// stx
		{0x86, STX}, {0x96, STX}, {0x8e, STX},
		// sty 
		{0x84, STY}, {0x94, STY}, {0x8c, STY},
	};
	map = b;
}

cpu::~cpu()
{

}

void cpu::load_program_in_rom(std::vector<u8> opcodes) // to load program in prg_rom and save reference to the code 0xffc memory cell
{
	int counter = 0x8000;
	program_counter = counter;
	map->mem_write_16(0x8000, 0xfffc, true); // used for reset 
	for (auto i : opcodes)
	{
		map->mem_write(opcodes[counter - 0x8000], counter, true);
		counter++;
	}
}

void cpu::connect_bus(cart* c, ppu* p)
{
	map = new bus(c);
	map->connect_ppu_to_bus(p);
}

void cpu::run()
{
	int counter = 0;
	program_counter = 0x8000;
	while (counter != 1600)
	{
		u8 opcode = map->mem_read(program_counter, true);
		program_counter += 1;
		counter++;
		uint8_t op_cycles = (this->*functions[opcodes_func_mapping[opcode]])(modes[opcodes_addressing_mode_mapping[opcode]]);
		cycles += op_cycles;
		map->tick(op_cycles);
	}
}

void cpu::reset()
{
	accumulator = 0x00;
	x = 0x00;
	y = 0x00;
	processor_status = 0x00;

	program_counter = map->mem_read(0xfffc, true);
}

void cpu::irq()
{
	if ((processor_status & 0x04) == 0x00)
	{
		u16 page_index = 0x0100;
		map->mem_write((program_counter >> 8) & 0x00ff, page_index | static_cast<u16>(stack_pointer), true);
		stack_pointer--;
		map->mem_write(program_counter & 0x00ff, page_index | static_cast<u16>(stack_pointer), true);
		stack_pointer--;

		set_flags(flags::break_instruction, 0);
		set_flags(flags::overflow, 1);
		set_flags(flags::irq_disable, 1);

		map->mem_write(processor_status, page_index | static_cast<u16>(stack_pointer), true);
		stack_pointer--;

		program_counter = map->mem_read_16(0xfffe, true);
	}
}

void cpu::nmi()
{
	if ((processor_status & 0x04) == 0x00)
	{
		u16 page_index = 0x0100;
		map->mem_write((program_counter >> 8) & 0x00ff, page_index | static_cast<u16>(stack_pointer), true);
		stack_pointer--;
		map->mem_write(program_counter & 0x00ff, page_index | static_cast<u16>(stack_pointer), true);
		stack_pointer--;

		set_flags(flags::break_instruction, 0);				      
		set_flags(flags::overflow, 1);    
		set_flags(flags::irq_disable, 1);    

		map->mem_write(processor_status, page_index | static_cast<u16>(stack_pointer), true);
		stack_pointer--;

		program_counter = map->mem_read_16(0xfffa, true);
	}
}

uint8_t cpu::lda(addressing_mode mode)
{
	std::cout << "LDA" << std::endl;

	u16 addr = get_operand_address(mode);

	accumulator = map->mem_read(addr, true);

	set_flags(flags::negative, (accumulator & 0x80) == 0x80);
	set_flags(flags::zero, accumulator == 0x00);

	program_counter += 1;

	return cycle(mode, true);

}

uint8_t cpu::adc(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("ADC \n");

	value_holder = map->mem_read(addr, true);
	prev_accumulator = accumulator;
	accumulator = accumulator + value_holder + (processor_status & 0x01);

	set_flags(flags::negative, (accumulator & 0x80) == 0x80);
	set_flags(flags::overflow, (accumulator < value_holder) | (accumulator < prev_accumulator));
	set_flags(flags::zero, accumulator == 0x00);
	set_flags(flags::carry, ((accumulator <= prev_accumulator) && (prev_accumulator != 0) && (value_holder != 0)));

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::bitwise_and(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("AND \n");

	value_holder = map->mem_read(addr, true);
	prev_accumulator = accumulator;
	accumulator &= value_holder;

	set_flags(flags::negative, (accumulator & 0x80) == 0x80);
	set_flags(flags::zero, accumulator == 0x00);

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::asl(addressing_mode mode)
{
	printf("ASL \n");

	if (mode == addressing_mode::accumulator)
	{
		set_flags(flags::carry, (accumulator & 0x80) >> 7);

		accumulator <<= 1;
	}
	else
	{
		u16 addr = get_operand_address(mode);

		value_holder = map->mem_read(addr, true);

		set_flags(flags::carry, (value_holder & 0x80) >> 7);

		value_holder <<= 1;

		map->mem_write(value_holder, addr, true);

		program_counter += 1;
	}

	set_flags(flags::negative, (accumulator & 0x80) == 0x80);
	set_flags(flags::zero, accumulator == 0x00);

	return cycle(mode, false);
}

uint8_t cpu::bit(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("BIT \n");

	value_holder = map->mem_read(addr, true);

	set_flags(flags::zero, value_holder & accumulator);
	set_flags(flags::negative, value_holder & 0x80); // n -> msb bit
	set_flags(flags::overflow, value_holder & 0x40); // v -> 2nd msb bit

	program_counter += 1;

	// return cycle(mode, false);
	return -1;
}

uint8_t cpu::cmp(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("CMP \n");

	value_holder = map->mem_read(addr, true);	

	set_flags(flags::zero, accumulator == value_holder);
	set_flags(flags::negative, ((accumulator - value_holder) & 0x80) == 0x80);
	set_flags(flags::carry, accumulator >= value_holder);

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::cpx(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("CPX \n");

	value_holder = map->mem_read(addr, true);

	set_flags(flags::zero, x == value_holder);
	set_flags(flags::negative, ((x - value_holder) & 0x80) == 0x80);
	set_flags(flags::carry, x >= value_holder);

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::cpy(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("CPY \n");

	value_holder = map->mem_read(addr, true);

	set_flags(flags::zero, y == value_holder);
	set_flags(flags::negative, ((y - value_holder) & 0x80) == 0x80);
	set_flags(flags::carry, y >= value_holder);

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::dec(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("DEC \n");

	value_holder = map->mem_read(addr, true);
	value_holder--;

	map->mem_write(value_holder, addr, true);

	set_flags(flags::negative, (value_holder & 0x80) == 0x80);
	set_flags(flags::zero, value_holder == 0x00);

	program_counter += 1;

	return cycle(mode, false);
}

uint8_t cpu::eor(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("EOR \n");

	value_holder = map->mem_read(addr, true);
	prev_accumulator = accumulator;
	accumulator ^= value_holder;

	set_flags(flags::negative, (value_holder & 0x80) == 0x80);
	set_flags(flags::zero, value_holder == 0x00);

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::inc(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("INC \n");

	value_holder = map->mem_read(addr, true);
	value_holder++;
	map->mem_write(value_holder, addr, true);

	set_flags(flags::negative, (value_holder & 0x80) == 0x80);
	set_flags(flags::zero, value_holder == 0x00);

	program_counter += 1;

	return cycle(mode, false);
}

uint8_t cpu::ldx(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("LDX \n");

	x = map->mem_read(addr, true);

	set_flags(flags::negative, (x & 0x80) == 0x80);
	set_flags(flags::zero, x == 0x00);

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::ldy(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("LDY \n");

	y = map->mem_read(addr, true);

	set_flags(flags::negative, (y & 0x80) == 0x80);
	set_flags(flags::zero, y == 0x00);

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::lsr(addressing_mode mode)
{
	printf("LSR \n");

	if (mode == addressing_mode::accumulator)
	{
		value_holder = accumulator;
		u8 temp_hld = value_holder & 0x01;

		accumulator >= 1;

		set_flags(flags::negative, false); // n cleared
		set_flags(flags::zero, value_holder == 0x00);

		processor_status = temp_hld; // low bit becomes carry: set if low bit was set; cleared if low bit was zero
	}
	else
	{
		u16 addr = get_operand_address(mode);
		value_holder = map->mem_read(addr, true);
		u8 temp_hld = value_holder & 0x01;

		value_holder >= 1;

		map->mem_write(value_holder, addr, true);

		set_flags(flags::negative, false); // n cleared
		set_flags(flags::zero, value_holder == 0x00);

		processor_status = temp_hld; // low bit becomes carry: set if low bit was set; cleared if low bit was zero
		program_counter += 1;
	}
	
	return cycle(mode, false);
}

uint8_t cpu::nop(addressing_mode mode)
{
	printf("NOP \n");
	for (u8 i = 0; i < 2; i++);
		// clock++;

	return 2;
}

uint8_t cpu::ora(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("ORA \n");

	value_holder = map->mem_read(addr, true);
	prev_accumulator = accumulator;   // ??????
	accumulator |= value_holder;

	set_flags(flags::negative, (accumulator & 0x80) == 0x80);
	set_flags(flags::zero, accumulator == 0x00);

	program_counter++;

	return cycle(mode, true);
}

uint8_t cpu::rol(addressing_mode mode)
{
	printf("ROL \n");
	if (mode == addressing_mode::accumulator)
	{
		value_holder = processor_status;
		u8 temp_acc = accumulator;

		set_flags(flags::carry, (accumulator & 0x80) >> 7);

		accumulator <<= 1; // shift left accumulator
		accumulator |= (value_holder & 0x01);  // put carry flag bit to accumulator lsb 

		set_flags(flags::negative, temp_acc & 80);
		set_flags(flags::zero, accumulator == 0x00);

		print_contents();
	}
	else
	{
		u16 addr = get_operand_address(mode);

		value_holder = map->mem_read(addr, true);
		u8 temp_value_holder = value_holder;
		u8 temp_processor_status = processor_status;

		set_flags(flags::carry, (value_holder & 0x80) >> 7);// put msb to carray bit of flag
		
		value_holder = (value_holder << 1);  // shift left accumulator
		value_holder |= (temp_processor_status & 0x01); // put carry flag bit to accumulator lsb 
		
		set_flags(flags::negative, temp_value_holder & 80);
		set_flags(flags::zero, value_holder == 0x00);

		program_counter++;
	}

	return cycle(mode, false);
}

uint8_t cpu::ror(addressing_mode mode)
{
	printf("ROR \n");
	if (mode == addressing_mode::accumulator)
	{
		value_holder = processor_status;
		u8 temp_acc = accumulator;

		set_flags(flags::carry, accumulator * 0x01);// put lsb to carry

		accumulator >>= 1; // shift accumulator right
		accumulator |= ((value_holder & 0x01) << 7); // put carry flag bit to accumulator msb
		processor_status |= (temp_acc & 0x80); // negative flag

		set_flags(flags::negative, temp_acc & 0x80);
		set_flags(flags::zero, value_holder == 0x00);
	}
	else
	{
		u16 addr = get_operand_address(mode);
		value_holder = map->mem_read(addr, true);
		u8 temp_value_holder = value_holder;
		u8 temp_processor_status = processor_status;

		set_flags(flags::carry, value_holder & 0x01); // put msb to carray bit of flag
		
		value_holder = (value_holder << 1);  // shift left accumulator
		value_holder |= (temp_processor_status & 0x01) << 7; // put carry flag bit to accumulator lsb 
		
		set_flags(flags::negative, temp_value_holder & 0x80);
		set_flags(flags::zero, value_holder == 0x00);

		program_counter++;
	}

	return cycle(mode, false);
}

uint8_t cpu::sbc(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("SBC \n");

	value_holder = map->mem_read(addr, true) ^ 0xff; // xor to invert value
	prev_accumulator = accumulator;
	accumulator = accumulator + value_holder + (processor_status & 0x01);

	set_flags(flags::negative, (accumulator & 0x80) == 0x80);
	set_flags(flags::overflow, (accumulator < value_holder) | (accumulator < prev_accumulator));
	set_flags(flags::carry, (accumulator <= prev_accumulator) && (prev_accumulator != 0) && (value_holder != 0));
	set_flags(flags::zero, accumulator == 0x00);

	program_counter += 1;

	return cycle(mode, true);
}

uint8_t cpu::sta(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("STA \n");

	map->mem_write(accumulator, addr, true);

	program_counter += 1;

	// quite different scheme
	return -1;
}

uint8_t cpu::stx(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("STX \n");

	map->mem_write(x, addr, true);

	program_counter += 1;

	return -1;
}

uint8_t cpu::sty(addressing_mode mode)
{
	u16 addr = get_operand_address(mode);

	printf("STY\n");

	map->mem_write(y, addr, true);

	program_counter += 1;

	return -1;
}

uint8_t cpu::bcc(addressing_mode mode)
{
	printf("BCC \n");
	if ((processor_status & 0x01) == 0x00)
	{
		u8 operand = map->mem_read(program_counter, true);
		program_counter = program_counter + (static_cast<u16>(operand ^ 0xff00)) - 1;
	}

	return 2;
}

uint8_t cpu::bcs(addressing_mode mode)
{
	printf("BCS \n");
	if ((processor_status & 0x01) == 0x01)
	{
		u8 operand = map->mem_read(program_counter, true);
		program_counter = program_counter + (static_cast<u16>(operand ^ 0xff00)) - 1;
	}

	return 2;
}

uint8_t cpu::beq(addressing_mode mode)
{
	printf("BEQ \n");
	if ((processor_status & 0x02) == 0x02)
	{
		u8 operand = map->mem_read(program_counter, true);
		program_counter = program_counter + (static_cast<u16>(operand ^ 0xff00)) - 1;
	}

	return 2;
}

uint8_t cpu::bmi(addressing_mode mode)
{
	printf("BMI \n");
	if ((processor_status & 0x80) == 0x80)
	{
		u8 operand = map->mem_read(program_counter, true);
		program_counter = program_counter + (static_cast<u16>(operand ^ 0xff00)) - 1;
	}

	return 2;
}

uint8_t cpu::bne(addressing_mode mode)
{
	printf("BNE \n");
	if ((processor_status & 0x02) == 0x00)
	{
		u8 operand = map->mem_read(program_counter, true);
		program_counter = program_counter + (static_cast<u16>(operand ^ 0xff00)) - 1;
	}

	return 2;
}

uint8_t cpu::bpl(addressing_mode mode)
{
	printf("BPL \n");
	if ((processor_status & 0x80) == 0x00)
	{
		u8 operand = map->mem_read(program_counter, true);
		program_counter = program_counter + (static_cast<u16>(operand ^ 0xff00)) - 1;
	}

	return 2;
}

uint8_t cpu::bvc(addressing_mode mode)
{
	printf("BVC \n");
	if ((processor_status & 0x40) == 0x00)
	{
		u8 operand = map->mem_read(program_counter, true);
		program_counter = program_counter + (static_cast<u16>(operand ^ 0xff00)) - 1;
	}

	return 2;
}

uint8_t cpu::bvs(addressing_mode mode)
{
	printf("BVS \n");
	if ((processor_status & 0x40) == 0x40)
	{
		u8 operand = map->mem_read(program_counter, true);
		program_counter = program_counter + (static_cast<u16>(operand ^ 0xff00)) - 1;
	}

	return 2;
}

uint8_t cpu::rti(addressing_mode mode)
{
	printf("RTI \n");

	u16 page_index = 0x0100;

	stack_pointer++;
	processor_status = map->mem_read(page_index | static_cast<u16>(stack_pointer), true);
	
	set_flags(flags::overflow, 0);
	set_flags(flags::break_instruction, 0);

	stack_pointer++;
	program_counter = static_cast<u16>(map->mem_read(page_index | static_cast<u16>(stack_pointer), true));
	stack_pointer++;
	program_counter |= static_cast<u16>(map->mem_read(page_index | static_cast<u16>(stack_pointer), true)) << 8;

	return 6;
}

uint8_t cpu::rts(addressing_mode mode)
{
	printf("RTS \n");

	u16 page_index = 0x0100;

	stack_pointer++;
	program_counter = static_cast<u16>(map->mem_read(page_index | static_cast<u16>(stack_pointer), true));
	stack_pointer++;
	program_counter |= static_cast<u16>(map->mem_read(page_index | static_cast<u16>(stack_pointer), true)) << 8;

	// depends on the value of pc when subroutine was called for now let's do
	program_counter++;

	return 6;
}

uint8_t cpu::brk(addressing_mode mode)
{
	printf("BRK \n");

	set_flags(flags::irq_disable, 1);
	map->mem_write((program_counter >> 8) & 0x00ff, 0x0100 + stack_pointer, true);
	stack_pointer--;
	map->mem_write(program_counter && 0x00ff, 0x0100 + stack_pointer, true);
	stack_pointer--;

	set_flags(flags::break_instruction, 1);
	map->mem_write(processor_status, 0x0100 + stack_pointer, true);
	stack_pointer--;
	set_flags(flags::break_instruction, 0);

	program_counter = map->mem_read_16(0xfffe, true);

	return 7;
}

uint8_t cpu::clc(addressing_mode mode)
{
	printf("CLC \n");
	set_flags(flags::carry, false);

	return 2;
}

uint8_t cpu::sec(addressing_mode mode)
{
	printf("SEC \n");
	set_flags(flags::carry, true);

	return 2;
}

uint8_t cpu::cli(addressing_mode mode)
{
	printf("CLI \n");
	set_flags(flags::irq_disable, false);

	return 2;
}

uint8_t cpu::sei(addressing_mode mode)
{
	printf("SEI \n");
	set_flags(flags::irq_disable, true);

	return 2;
}

uint8_t cpu::clv(addressing_mode mode)
{
	printf("CLV \n");
	set_flags(flags::overflow, false);

	return 2;
}

uint8_t cpu::cld(addressing_mode mode)
{
	printf("CLD \n");
	set_flags(flags::decimal_mode, false);

	return 2;
}

uint8_t cpu::sed(addressing_mode mode)
{
	printf("SED \n");
	set_flags(flags::decimal_mode, true);

	return 2;
}

uint8_t cpu::jmp(addressing_mode mode)
{
	printf("JMP \n");

	u16 address = map->mem_read_16(program_counter, true);
	program_counter = address;

	return -1;
}

uint8_t cpu::jsr(addressing_mode mode)
{
	printf("JSR \n");
	u16 value = map->mem_read_16(program_counter, true);

	program_counter+=2;

	map->mem_write((program_counter >> 8) & 0x00ff, 0x0100 + stack_pointer, true);
	stack_pointer--;
	map->mem_write(program_counter && 0x00ff, 0x0100 + stack_pointer, true);
	stack_pointer--;

	program_counter = value;

	return -1;
}

uint8_t cpu::txs(addressing_mode mode)
{
	printf("TXS \n");
	stack_pointer = x;

	return 2;
}

uint8_t cpu::tsx(addressing_mode mode)
{
	printf("TSX \n");
	x = stack_pointer;

	set_flags(flags::zero, x == 0x00);
	set_flags(flags::negative, x & 0x80);

	return 2;
}

uint8_t cpu::pha(addressing_mode mode)
{
	printf("PHA \n");

	map->mem_write(accumulator, 0x0100 + stack_pointer, true);
	stack_pointer--;

	return 3;
}

uint8_t cpu::pla(addressing_mode mdoe)
{
	printf("PLA \n");

	stack_pointer++;
	accumulator = map->mem_read(0x0100 + stack_pointer, true);

	set_flags(flags::zero, accumulator == 0x00);
	set_flags(flags::negative, accumulator & 0x80);

	return 4;
}

uint8_t cpu::php(addressing_mode mode)
{
	printf("PHP \n");

	map->mem_write(0x0100 + stack_pointer, processor_status | 0x08, true);

	set_flags(flags::break_instruction, 0);
	set_flags(flags::overflow, 0);

	stack_pointer--;

	return 3;
}

uint8_t cpu::plp(addressing_mode mode)
{
	printf("PLP \n");

	stack_pointer++;
	accumulator = map->mem_read(0x0100 + stack_pointer, true);

	set_flags(flags::zero, accumulator == 0x00);
	set_flags(flags::negative, accumulator & 0x80);

	return 4;
}

uint8_t cpu::tax(addressing_mode mode)
{
	printf("TAX \n");

	x = accumulator;

	set_flags(flags::zero, x == 0x00);
	set_flags(flags::negative, x & 0x80);

	return 2;
}

uint8_t cpu::txa(addressing_mode mode)
{
	printf("TXA \n");

	accumulator = x;

	set_flags(flags::zero, accumulator == 0x00);
	set_flags(flags::negative, accumulator & 0x80);

	return 2;
}

uint8_t cpu::dex(addressing_mode mode)
{
	printf("DEX \n");

	x--;

	set_flags(flags::zero, x == 0x00);
	set_flags(flags::negative, x & 0x80);

	return 2;
}

uint8_t cpu::inx(addressing_mode mode)
{
	printf("INX \n");

	x++;

	set_flags(flags::zero, x == 0x00);
	set_flags(flags::negative, x & 0x80);

	return 2;
}

uint8_t cpu::tay(addressing_mode mode)
{
	printf("TAY \n");

	y = accumulator;

	set_flags(flags::zero, y == 0x00);
	set_flags(flags::negative, y & 0x80);

	return 2;
}

uint8_t cpu::tya(addressing_mode mode)
{
	printf("TYA \n");

	accumulator = y;

	set_flags(flags::zero, accumulator == 0x00);
	set_flags(flags::negative, accumulator & 0x80);

	return 2;
}

uint8_t cpu::dey(addressing_mode mode)
{
	printf("DEY \n");

	y--;

	set_flags(flags::zero, y == 0x00);
	set_flags(flags::negative, y & 0x80);

	return 2;
}

uint8_t cpu::iny(addressing_mode mode)
{
	printf("INY \n");

	y++;

	set_flags(flags::zero, y == 0x00);
	set_flags(flags::negative, y & 0x80);

	return 2;
}

u16 cpu::get_operand_address(addressing_mode mode)
{
	return (this->*addr_mode[static_cast<u8>(mode)])();
}

void cpu::set_flags(flags f, bool cond)
{
	if (cond)
	{
		processor_status |= static_cast<u8>(f);
	}
	else
	{
		processor_status &= (static_cast<u8>(f) ^ 0xff);
	}
}

u16 cpu::immediate()
{
	return program_counter;
}

u16 cpu::zero_page()
{
	return static_cast<u16>(map->mem_read(program_counter, true));
}

u16 cpu::zero_page_x()
{
	u16 pos = map->mem_read(program_counter, true);
	return pos + static_cast<u16>(x);
}

u16 cpu::zero_page_y()
{
	u16 pos = map->mem_read(program_counter, true);
	return pos + static_cast<u16>(y);
}

u16 cpu::absolute()
{
	program_counter++;
	return map->mem_read_16(program_counter - 1, true);
}

u16 cpu::absolute_x()
{
	u16 base = map->mem_read_16(program_counter, true);
	program_counter++;
	return base + static_cast<u16>(x);
}

u16 cpu::absolute_y()
{
	u16 base = map->mem_read_16(program_counter, true);
	program_counter++;
	return base + static_cast<u16>(y);
}

u16 cpu::indirect_x()
{
	u8 base = map->mem_read(program_counter, true);
	u8 ptr = base + x;
	u8 lo = map->mem_read(static_cast<u16>(ptr), true);
	u8 hi = map->mem_read(static_cast<u16> (ptr + 1), true);

	return hi << 8 | static_cast<u16>(lo);
}

u16 cpu::indirect_y()
{
	u16 base = map->mem_read(program_counter, true);

	u8 lo = map->mem_read(static_cast<u16>(base), true);
	u16 hi = map->mem_read(static_cast<u16> (base + 1), true);
	u16 deref_base = hi << 8 | static_cast<u16>(lo);

	return  hi << 8 | static_cast<u16>(lo);
}
