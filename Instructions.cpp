
#include "Instructions.h"

instruction::instruction(int64_t instruction)
{
	bit_representation = instruction;
	opcode = static_cast<instructions>( bit_representation >> 59);
	destination = (bit_representation >> 53) & 63;//preset under assumption destination register needed, will be ignored on instructions that don't require it.
	operand_1 = (bit_representation >> 47) & 63;//preset under assumption operand 1 is register, will be ignored on overwritten otherwise.
	operand_2 = (bit_representation >> 41) & 63;//preset under assumption operand 2 is register, will be ignored on overwritten otherwise.

	switch (opcode)
	{
	case instructions::NOP://Ignore dest, op1 and op2.
	case instructions::ADDR://all ops set as needed
	case instructions::MULR://all ops set as needed
	case instructions::CMPR://all ops set as needed
	case instructions::BRR://destination indicates source register for jump
	case instructions::LDRR://all ops set as needed
	case instructions::STOR:
		break;
	case instructions::ADDI:
	case instructions::MULI:
	case instructions::CMPI:
	case instructions::LDR:
	case instructions::STO:
	case instructions::BRI:
		operand_2 = ((bit_representation << 17) >> 17);//operand 2 is a literal so remove the instruction, destination and operand 1 bits.
		break;
	case instructions::LDC:
	case instructions::JMPR:
		operand_1 = ((bit_representation << 11) >> 11);//operand 1 is a literal so remove the instruction, destination bits, no operand 2.
		break;
	case instructions::JMP:
		operand_1 = ((bit_representation << 5) >> 5);//operand 1 is a literal and destination is implicit pc so remove the opcode.
		destination = register_names::program_counter;
		break;
	default:
		break;
	}
}

instruction::instruction(instructions instruction, register_names Dest, int64_t operand_1_s, int64_t operand_2_s)
{
	opcode = instruction;
	destination = Dest;
	this->operand_1 = operand_1;
	this->operand_2 = operand_2;
	bit_representation = static_cast<int64_t>(opcode) << 59;
	auto operand_1 = static_cast<uint64_t>(operand_1_s);
	auto operand_2 = static_cast<uint64_t>(operand_2_s);
	switch (opcode)
	{
	case instructions::NOP://Ignore dest, op1 and op2.
	case instructions::ADDR://all ops set as needed
	case instructions::MULR://all ops set as needed
	case instructions::CMPR://all ops set as needed
	case instructions::BRR://Branch to dest if reg1 == reg2
	case instructions::LDRR://all ops set as needed
	case instructions::STOR:
		bit_representation = bit_representation ^ (destination << 53);//add destination register
		bit_representation = bit_representation ^ (operand_1 << 47);//add operand_1 register
		bit_representation = bit_representation ^ (operand_2 << 41);//add operand_2 register
		break;
	case instructions::ADDI:
	case instructions::MULI:
	case instructions::CMPI:
	case instructions::LDR:
	case instructions::STO:
	case instructions::BRI:///Branch to dest if op1 == immediate2
		bit_representation = bit_representation ^ (destination << 53);//add destination register
		bit_representation = bit_representation ^ (operand_1 << 47);//add operand_1 register
		bit_representation = bit_representation ^ ((operand_2 << 17) >> 17);//add operand_2 immediate

		break;
	case instructions::LDC:
	case instructions::JMPR:
		bit_representation = bit_representation ^ (destination << 53);//add destination register
		bit_representation = bit_representation ^ ((operand_1 << 11) >> 11);//operand 1 is a literal so remove the instruction, destination bits, no operand 2.
		break;
	case instructions::JMP:
		bit_representation = bit_representation ^ ((operand_1 << 5) >> 5);//operand 1 is a literal and destination is implicit pc so remove the opcode.
		break;
	default:
		break;
	}
}

int64_t instruction::get_bit_representation()
{
	return bit_representation;
}

instructions instruction::get_opcode()
{
	return opcode;
}

register_names instruction::get_destination()
{
	return static_cast<register_names>(destination);
}

int64_t instruction::get_operand_1()
{
	return operand_1;
}

int64_t instruction::get_operand_2()
{
	return operand_2;
}

void instruction::print(std::ostream & os)
{
	switch (opcode)
	{
	case instructions::NOP://Ignore dest, op1 and op2.
	case instructions::ADDR://all ops set as needed
	case instructions::MULR://all ops set as needed
	case instructions::CMPR://all ops set as needed
	case instructions::LDRR://all ops set as needed
	case instructions::BRR://Branch to dest if reg1 == reg2
	case instructions::STOR://Branch to dest if reg1 == reg2
		os<< physical_register::register_name_to_string((register_names)destination)<<" = "<< opcode_to_name(opcode).c_str() << " " << physical_register::register_name_to_string((register_names)operand_1) << " " << physical_register::register_name_to_string((register_names)operand_2)<<'\n';
		break;
	case instructions::ADDI:
	case instructions::MULI:
	case instructions::CMPI:
	case instructions::LDR:
	case instructions::STO:
	case instructions::BRI:///Branch to dest if op1 == immediate2
		os << physical_register::register_name_to_string((register_names)destination) << " = " << opcode_to_name(opcode).c_str() << " " << physical_register::register_name_to_string((register_names)operand_1) << " " << operand_2 << '\n';


		break;
	case instructions::LDC:
		os << physical_register::register_name_to_string((register_names)destination) << " = " << opcode_to_name(opcode).c_str() << " " << operand_1 << '\n';
		break;
	case instructions::JMPR:
		os <<  opcode_to_name(opcode).c_str() << " " << physical_register::register_name_to_string((register_names)destination) << " + "  << operand_1 << '\n';

		break;
	case instructions::JMP:
		os << opcode_to_name(opcode).c_str() << " " << operand_1 << '\n';

		break;
	default:
		os << "error!\n";
		break;
	}
}

std::string instruction::opcode_to_name(instructions i)
{
	switch (i)
	{
	case instructions::NOP:
		return "NOP";
	case instructions::ADDR:
		return "ADDR";
	case instructions::ADDI:
		return "ADDI";
	case instructions::MULR:
		return "MULR";
	case instructions::MULI:
		return "MULI";
	case instructions::CMPR:
		return "CMPR";
	case instructions::CMPI:
		return "CMPI";
	case instructions::LDR:
		return "LDR";
	case instructions::LDRR://all ops set as needed
		return "LDRR";
	case instructions::LDC:
		return "LDC";
	case instructions::BRI:
		return "BRI";
	case instructions::BRR:
		return "BRR";
	case instructions::JMP:
		return "JMP";
	case instructions::JMPR:
		return "JMPR";
	case instructions::STO:
		return "STO";
	case instructions::STOR:
		return "STOR";
	}
	return "Unknown?!";
}
