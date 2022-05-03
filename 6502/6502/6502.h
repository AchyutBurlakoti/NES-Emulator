#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "ppu.h"
#include "cart.h"

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

class bus;

class cpu
{
	u8 accumulator;
	u8 x, y;
	u8 stack_pointer;
	u8 processor_status;
	u16 program_counter;

	bus* map;

	std::map<int, int> opcodes_addressing_mode_mapping;
	std::map<int, int> opcodes_func_mapping;

	int value_holder;
	u8 prev_accumulator;

	int cycles;
	uint8_t cycle_buf;

	addressing_mode modes[11];

	// bit manipulatio instruction
	uint8_t lda(addressing_mode mode);
	uint8_t adc(addressing_mode mode);
	uint8_t bitwise_and(addressing_mode mode);
	uint8_t asl(addressing_mode mode);
	uint8_t bit(addressing_mode mode);
	uint8_t cmp(addressing_mode mode);
	uint8_t cpx(addressing_mode mode);
	uint8_t cpy(addressing_mode mode);
	uint8_t dec(addressing_mode mode);
	uint8_t eor(addressing_mode mode);
	uint8_t inc(addressing_mode mode);
	uint8_t ldx(addressing_mode mode);
	uint8_t ldy(addressing_mode mode);
	uint8_t lsr(addressing_mode mode);
	uint8_t nop(addressing_mode mode);
	uint8_t ora(addressing_mode mode);
	uint8_t rol(addressing_mode mode);
	uint8_t ror(addressing_mode mode);
	uint8_t sbc(addressing_mode mode);
	uint8_t sta(addressing_mode mode);
	uint8_t stx(addressing_mode mode);
	uint8_t sty(addressing_mode mode);

	// branching instruction
	uint8_t bcc(addressing_mode mode);
	uint8_t bcs(addressing_mode mode);
	uint8_t beq(addressing_mode mode);
	uint8_t bmi(addressing_mode mode);
	uint8_t bne(addressing_mode mode);
	uint8_t bpl(addressing_mode mode);
	uint8_t bvc(addressing_mode mode);
	uint8_t bvs(addressing_mode mode);

	// interrupt instruction
	uint8_t rti(addressing_mode mode);
	uint8_t rts(addressing_mode mode);
	uint8_t brk(addressing_mode mode);

	// Flags Instruction
	uint8_t clc(addressing_mode mode);
	uint8_t sec(addressing_mode mode);
	uint8_t cli(addressing_mode mode);
	uint8_t sei(addressing_mode mode);
	uint8_t clv(addressing_mode mode);
	uint8_t cld(addressing_mode mode);
	uint8_t sed(addressing_mode mode);

	// branching instruction
	uint8_t jmp(addressing_mode mode);
	uint8_t jsr(addressing_mode mode);

	// stack instruction
	uint8_t txs(addressing_mode mode);
	uint8_t tsx(addressing_mode mode);
	uint8_t pha(addressing_mode mode);
	uint8_t pla(addressing_mode mdoe);
	uint8_t php(addressing_mode mode);
	uint8_t plp(addressing_mode mode);

	// register instruction
	uint8_t tax(addressing_mode mode);
	uint8_t txa(addressing_mode mode);
	uint8_t dex(addressing_mode mode);
	uint8_t inx(addressing_mode mode);
	uint8_t tay(addressing_mode mode);
	uint8_t tya(addressing_mode mode);
	uint8_t dey(addressing_mode mode);
	uint8_t iny(addressing_mode mode);

	typedef uint8_t (cpu::*opcodes_function) (addressing_mode mode);
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

	cpu(bus* b);
	~cpu();
	void load_program_in_rom(std::vector<u8> opcodes);
	void connect_bus(cart* c, ppu* p);
	void run();
	void reset();
	void irq();
	void nmi();
	void print_contents();

	uint8_t cycle(addressing_mode mode, bool simple);
};