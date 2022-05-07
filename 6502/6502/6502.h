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

using OPCODE_RT = void;

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
	u8 cycle_buf;

	addressing_mode modes[11];

	// bit manipulatio instruction
	OPCODE_RT lda(addressing_mode mode);
	OPCODE_RT adc(addressing_mode mode);
	OPCODE_RT bitwise_and(addressing_mode mode);
	OPCODE_RT asl(addressing_mode mode);
	OPCODE_RT bit(addressing_mode mode);
	OPCODE_RT cmp(addressing_mode mode);
	OPCODE_RT cpx(addressing_mode mode);
	OPCODE_RT cpy(addressing_mode mode);
	OPCODE_RT dec(addressing_mode mode);
	OPCODE_RT eor(addressing_mode mode);
	OPCODE_RT inc(addressing_mode mode);
	OPCODE_RT ldx(addressing_mode mode);
	OPCODE_RT ldy(addressing_mode mode);
	OPCODE_RT lsr(addressing_mode mode);
	OPCODE_RT nop(addressing_mode mode);
	OPCODE_RT ora(addressing_mode mode);
	OPCODE_RT rol(addressing_mode mode);
	OPCODE_RT ror(addressing_mode mode);
	OPCODE_RT sbc(addressing_mode mode);
	OPCODE_RT sta(addressing_mode mode);
	OPCODE_RT stx(addressing_mode mode);
	OPCODE_RT sty(addressing_mode mode);

	// branching instruction
	OPCODE_RT bcc(addressing_mode mode);
	OPCODE_RT bcs(addressing_mode mode);
	OPCODE_RT beq(addressing_mode mode);
	OPCODE_RT bmi(addressing_mode mode);
	OPCODE_RT bne(addressing_mode mode);
	OPCODE_RT bpl(addressing_mode mode);
	OPCODE_RT bvc(addressing_mode mode);
	OPCODE_RT bvs(addressing_mode mode);

	// interrupt instruction
	OPCODE_RT rti(addressing_mode mode);
	OPCODE_RT rts(addressing_mode mode);
	OPCODE_RT brk(addressing_mode mode);

	// Flags Instruction
	OPCODE_RT clc(addressing_mode mode);
	OPCODE_RT sec(addressing_mode mode);
	OPCODE_RT cli(addressing_mode mode);
	OPCODE_RT sei(addressing_mode mode);
	OPCODE_RT clv(addressing_mode mode);
	OPCODE_RT cld(addressing_mode mode);
	OPCODE_RT sed(addressing_mode mode);

	// branching instruction
	OPCODE_RT jmp(addressing_mode mode);
	OPCODE_RT jsr(addressing_mode mode);

	// stack instruction
	OPCODE_RT txs(addressing_mode mode);
	OPCODE_RT tsx(addressing_mode mode);
	OPCODE_RT pha(addressing_mode mode);
	OPCODE_RT pla(addressing_mode mdoe);
	OPCODE_RT php(addressing_mode mode);
	OPCODE_RT plp(addressing_mode mode);

	// register instruction
	OPCODE_RT tax(addressing_mode mode);
	OPCODE_RT txa(addressing_mode mode);
	OPCODE_RT dex(addressing_mode mode);
	OPCODE_RT inx(addressing_mode mode);
	OPCODE_RT tay(addressing_mode mode);
	OPCODE_RT tya(addressing_mode mode);
	OPCODE_RT dey(addressing_mode mode);
	OPCODE_RT iny(addressing_mode mode);

	typedef OPCODE_RT (cpu::*opcodes_function) (addressing_mode mode);
	opcodes_function functions[56];

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
	void connect_bus(cart* c, ppu* p);
	void run();
	void reset();
	void irq();
	void nmi();
	void print_contents();

	u8 cycle(addressing_mode mode, bool simple);
};