
#include "test_register.h"
#include "Register.h"
bool test_equalities()
{
	physical_register test_reg{ register_names::program_counter,100lu };
	if ((test_reg.get_register_name() != register_names::program_counter) || (test_reg.get_register_value() != 100lu))
	{
		std::cout << "Register Equalities Failed, expected program counter with value 100, received: ";
		test_reg.print(std::cout);
		std::cout << "\n";
		return false;
	}
	std::cout << "Register Equalities Passed\n";
	return true;

}

bool test_clock_tick()
{
	physical_register test_reg{ register_names::program_counter,100 };
	test_reg.set_register_value(125lu);
	if (test_reg.get_register_value() != 100lu)
	{
		std::cout << "Register Clock Tick Failed, expected program counter with value 100, received: ";
		test_reg.print(std::cout);
		std::cout << "\n";
		return false;
	}
	test_reg.clock_tick();
	if (test_reg.get_register_value() != 125lu)
	{
		std::cout << "Register Clock Tick Failed, expected program counter with value 125, received: ";
		test_reg.print(std::cout);
		std::cout << "\n";
		return false;
	}
	std::cout << "Register Clock Tick Passed\n";
	return true;
}
bool test_registers()
{
	bool registers_clean = true;
	registers_clean&=test_equalities();
	registers_clean &= test_clock_tick();
	return registers_clean;
}
