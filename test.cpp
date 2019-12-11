
#include "test.h"
#include "test_register.h"
#include "test_mem.h"
#include "test_processor.h"
#include "test_instructions.h"
void test_functions()
{
	bool tests_passed_clean = true;
	tests_passed_clean&=test_registers();
	tests_passed_clean &= test_memory();
	tests_passed_clean &= test_processor();
	tests_passed_clean &= test_instructions_translations();
	if (tests_passed_clean)
	{
		std::cout << "All tests clean!\n";
	}
	else
	{
		std::cout << "TESTS FAILED!\n";
	}
}
