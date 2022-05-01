#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "bus.h"

#define LDA 0
#define ADC 1
#define AND 2
#define ASL 3
#define BIT 4
#define CMP 5
#define CPX 6
#define CPY 7
#define DEC 8
#define EOR 9
#define INC 10
#define LDX 11
#define LDY 12
#define LSR 13
#define NOP 14
#define ORA 15
#define ROL 16
#define ROR 17
#define SBC 18

#define BCC 19
#define BCS 20
#define BEQ 21
#define BMI 22
#define BNE 23
#define BPL 24
#define BVC 25
#define BVS 26

#define RTI 27
#define RTS 28
#define BRK 29

#define CLC 30
#define SEC 31
#define CLI 32
#define SEI 33
#define CLV 34
#define CLD 35
#define SED 36

#define JMP 37
#define JSR 38

#define TXS 39
#define TSX 40
#define PHA 41
#define PLA 42
#define PHP 43
#define PLP 44

#define TAX 45
#define TXA 46
#define DEX 47
#define INX 48
#define TAY 49
#define TYA 50
#define DEY 51
#define INY 52

#define STA 53
#define STX 54
#define STY 55

class mapper;

using u8 = uint8_t;
using u16 = uint16_t;

enum class addressing_mode : int
{
	immediate,
	zero_page,
	zero_page_x,
	zero_page_y,
	absolute,
	absolute_x,
	absolute_y,
	indirect_x,
	indirect_y,
	accumulator,
	no_address
};

enum class flags : int
{
	negative = 0x80,
	overflow = 0x40,
	break_instruction = 0x10,
	decimal_mode = 0x8,
	irq_disable = 0x4,
	zero = 0x2,
	carry = 0x1
};

enum switcher
{
	CPU,
	PPU
};

class cpu
{
	u8 accumulator;
	u8 x, y;
	u8 stack_pointer;
	u8 processor_status;
	u16 program_counter;

	mapper* map;

	std::map<int, int> opcodes_addressing_mode_mapping;
	std::map<int, int> opcodes_func_mapping;

	int value_holder;
	u8 prev_accumulator;

	addressing_mode modes[11];

	// bit manipulatio instruction
	void lda(addressing_mode mode);
	void adc(addressing_mode mode);
	void bitwise_and(addressing_mode mode);
	void asl(addressing_mode mode);
	void bit(addressing_mode mode);
	void cmp(addressing_mode mode);
	void cpx(addressing_mode mode);
	void cpy(addressing_mode mode);
	void dec(addressing_mode mode);
	void eor(addressing_mode mode);
	void inc(addressing_mode mode);
	void ldx(addressing_mode mode);
	void ldy(addressing_mode mode);
	void lsr(addressing_mode mode);
	void nop(addressing_mode mode);
	void ora(addressing_mode mode);
	void rol(addressing_mode mode);
	void ror(addressing_mode mode);
	void sbc(addressing_mode mode);
	void sta(addressing_mode mode);
	void stx(addressing_mode mode);
	void sty(addressing_mode mode);

	// branching instruction
	void bcc(addressing_mode mode);
	void bcs(addressing_mode mode);
	void beq(addressing_mode mode);
	void bmi(addressing_mode mode);
	void bne(addressing_mode mode);
	void bpl(addressing_mode mode);
	void bvc(addressing_mode mode);
	void bvs(addressing_mode mode);

	// interrupt instruction
	void rti(addressing_mode mode);
	void rts(addressing_mode mode);
	void brk(addressing_mode mode);

	// Flags Instruction
	void clc(addressing_mode mode);
	void sec(addressing_mode mode);
	void cli(addressing_mode mode);
	void sei(addressing_mode mode);
	void clv(addressing_mode mode);
	void cld(addressing_mode mode);
	void sed(addressing_mode mode);

	// branching instruction
	void jmp(addressing_mode mode);
	void jsr(addressing_mode mode);

	// stack instruction
	void txs(addressing_mode mode);
	void tsx(addressing_mode mode);
	void pha(addressing_mode mode);
	void pla(addressing_mode mdoe);
	void php(addressing_mode mode);
	void plp(addressing_mode mode);

	// register instruction
	void tax(addressing_mode mode);
	void txa(addressing_mode mode);
	void dex(addressing_mode mode);
	void inx(addressing_mode mode);
	void tay(addressing_mode mode);
	void tya(addressing_mode mode);
	void dey(addressing_mode mode);
	void iny(addressing_mode mode);

	typedef void (cpu::*opcodes_function) (addressing_mode mode);
	opcodes_function functions[58];

	u16 get_operand_address(addressing_mode mode);
	void set_flags(flags f, bool cond);

	u16 immediate();
	u16 zero_page();
	u16 zero_page_x();
	u16 zero_page_y();
	u16 absolute();
	u16 absolute_x();
	u16 absolute_y();
	u16 indirect_x();
	u16 indirect_y();

	typedef u16 (cpu::* addres_mode_func) ();
	addres_mode_func addr_mode[9];

public:

	cpu();
	~cpu();
	void load_program_in_rom(std::vector<u8> opcodes);
	void connect_bus();
	void run();
	void reset();
	void irq();
	void nmi();
	void print_contents();
};