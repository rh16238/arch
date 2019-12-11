
#include "test_instructions.h"
#include "Instructions.h"
#include "math_utils.h"
bool test_nop_translation()
{
	instruction test{ instructions::NOP,register_names::r0,0,0 };
	auto x = test.get_bit_representation();
	instruction match{ x };
	if (match.get_opcode() != instructions::NOP)
	{
		std::cout << "Error in NOP translation, NOP did not match\n";
		return false;
	}

	std::cout << "NOP translation passed\n";
	return true;
}
bool test_triple_register_translation()
{
	instruction test{ instructions::ADDR,register_names::r0,register_names::r1,register_names::r2 };
	auto x = test.get_bit_representation();
	instruction match{ x };
	if (match.get_opcode() != instructions::ADDR || match.get_destination() != register_names::r0 || match.get_operand_1() != register_names::r1|| match.get_operand_2() != register_names::r2)
	{
		std::cout << "Error in triple register translation, instruction did not match or registers did not match\n";
		return false;
	}

	std::cout << "Triple register translation passed\n";
	return true;
}
bool test_double_register_immediate_translation()
{
	instruction test{ instructions::ADDI,register_names::program_counter,register_names::r15,3214517lu};
	auto x = test.get_bit_representation();
	instruction match{ x };
	if (match.get_opcode() != instructions::ADDI || match.get_destination() != register_names::program_counter || match.get_operand_1() != register_names::r15 || match.get_operand_2() != 3214517lu)
	{
		std::cout << "Error in double register and immediate translation, instruction, registers or immediate mismatch\n";
		return false;
	}

	std::cout << "Double register and immediate translation passed\n";
	return true;
}
bool test_long_immediate_translation()
{
	instruction test{ instructions::LDC,register_names::program_counter,7199254740991lu, register_names::r0 };
	auto x = test.get_bit_representation();
	instruction match{ x };
	if (match.get_opcode() != instructions::LDC || match.get_destination() != register_names::program_counter || match.get_operand_1() != 7199254740991lu)
	{
		std::cout << "Error in register and long immediate translation, instruction, registers or immediate mismatch\n";
		return false;
	}

	std::cout << "Register and long immediate translation passed\n";
	return true;
}
bool test_long_long_immediate_translation()
{
	instruction test{ instructions::JMP,register_names::program_counter,460752303423487lu, register_names::r0 };
	auto x = test.get_bit_representation();
	instruction match{ x };
	if (match.get_opcode() != instructions::JMP ||match.get_operand_1() != 460752303423487lu)
	{
		std::cout << "Error in long long immediate translation, immediate or instruction mismatch\n";
		return false;
	}

	std::cout << "Long long immediate translation passed\n";
	return true;
}
bool test_instructions_translations()
{
	bool tests_clean = true;
	tests_clean &= test_nop_translation();
	tests_clean &= test_triple_register_translation();
	tests_clean &= test_double_register_immediate_translation();
	tests_clean &= test_long_immediate_translation();
	tests_clean &= test_long_long_immediate_translation();
	return tests_clean;
}
