#pragma once
#include "math_utils.h"
#include "Register.h"
enum instructions
{
	NOP,//No op
	ADDR,//Add Dest = REG1 + REG2
	ADDI,//ADD DEST = REG1 + IMMEDIATE
	MULR, //DEST = REG x REG
	MULI,//DEST = REG X IMMEDIATE
	CMPR,//DEST = (1 if reg1>reg2, 0 if reg1==reg2, -1 if reg1<reg2)
	CMPI,//DEST = (1 if reg1>immediate, 0 if reg1==immediate, -1 if reg1<immediate)
	LDR, //DEST = contents of memory address REG1 + Index
	LDRR, //DEST = contents of memory address REG1 + Reg2
	LDC,//DEST = Immediate
	BRI,//BRANCH to DEST if REG1 = IMM2
	BRR,//BRANCH TO DEST if REG1 = REG2
	JMP, //Jump to Constant address of IMMEDIATE
	JMPR, //jump to DEST + OP1
	STO, //DEST + IMMEDIATE2 = REG 1
	STOR, //DEST + REG1 = REG 2
};

class instruction
{
public:
	instruction(int64_t instruction);
	instruction(instructions instruction, register_names Dest, int64_t operand_1, int64_t operand_2);
	int64_t get_bit_representation();
	instructions get_opcode();
	register_names get_destination();
	int64_t get_operand_1();
	int64_t get_operand_2();
	void print(std::ostream& os);
	/*static*/ std::string opcode_to_name(instructions i);
private:
	int64_t bit_representation;
	instructions opcode;
	int64_t destination;
	int64_t operand_1;
	int64_t operand_2;
};