
#include <iostream>
#include "math_utils.h"
#include "test.h"
#include "program_store.h"
#include <fstream>
#include <string>
#include "processor_basic.h"
#include "processor_pipelined.h"
#include <iomanip>
void interrogate_processor(processor_pipelined p, program_list p_id, int problem_size, bool fin)
{
	std::cout << p.return_clock_time() << ": Program in interrogation mode \n";
	std::cout.flush();
	do
	{
		int integer_buffer = 0;
		std::string buffer;
		std::cin >> buffer;
		auto memory = p.return_memory();
		if (buffer == "stats")
		{
			std::cout << "STATS---------------\n";
			std::cout << "Clock cycles: " << p.return_clock_time() << '\n';
			std::cout << "Instructions Executed: " << p.return_executed_instructions() << '\n';
			std::cout << "idle cycles " << p.return_idle_time() << '\n';
			std::cout << "STATS-END-----------\n";
		}
		if (buffer == "print")
		{
			std::cout << "PRINT---------------\n";
			p.print(std::cout);
			std::cout << "PRINT-END-----------\n";
		}
		else if (buffer == "reg")
		{
			std::cin >> integer_buffer;
			auto val = p.return_register_value((register_names)integer_buffer);
			physical_register test{ (register_names)integer_buffer ,0 };

			std::cin >> buffer;
			std::cout << "REG-----------------\n";
			std::cout << test.register_name_to_string((register_names)integer_buffer)<<":";
			if (buffer == "I")
			{
				try
				{
					instruction i{ val };
					i.print(std::cout);
				}
				catch(...)
				{
					std::cout << "Value does not appear to be a instruction\n";
				}

			}
			else if (buffer == "X")
			{
				std::cout <<"0x"<< std::hex << val << '\n';
			}
			else
			{
				std::cout << val << '\n';
			}
			std::cout << "REG-END-------------\n";
		}
		else if (buffer == "mem")
		{
			std::cin >> integer_buffer;
			auto val = memory[integer_buffer];
			physical_register test{ (register_names)integer_buffer ,0 };

			std::cin >> buffer;
			std::cout << "MEM-----------------\n";
			std::cout <<"Mem Locoation " << integer_buffer << ":";
			if (buffer == "I")
			{
				try
				{
					instruction i{ val };
					i.print(std::cout);
				}
				catch (...)
				{
					std::cout << "Value does not appear to be a instruction\n";
				}

			}
			else if (buffer == "X")
			{
				std::cout << "0x" << std::hex << val << '\n';
			}
			else
			{
				std::cout << val << '\n';
			}
			std::cout << "MEM-END-------------\n";
		}
		else if (buffer == "c")
		{
			return;
		}
	} while (1);
}
int main(int argc, char* argv[])
{
	test_functions();
	for (size_t i = 0; i < argc; i++)
	{
		if (argv[i] == "-D")
		{
			test_functions();
			return 0;
		}
	}
	do
	{
		std::cout << "To run a program please use the following format: run [Program_Id] [-i Interactive Flag] [Variable]*\n";
		int int_buffer;
		std::string buffer;
		std::cin >> buffer;//ignore first 'run'
		std::cin >> int_buffer;
		program_list program = (program_list)int_buffer;

		std::cin >> buffer;
		bool interactive = (buffer == "-i");
		std::vector<int64_t> a, b;
		int64_t escape_point = 0;
		std::vector<int64_t> memory;
		int problem_size = 0;
		switch (program)
		{
		case vector_addition:
			std::cin >> problem_size;
			for (size_t i = 0; i < problem_size; i++)
			{
				std::cin >> int_buffer;
				a.push_back(int_buffer);
			}
			for (size_t i = 0; i < problem_size; i++)
			{
				std::cin >> int_buffer;
				b.push_back(int_buffer);
			}
			memory = program_vector_addition(a, b);
			escape_point = naive_vector_addition_exit_pc();
			break;
		case naive_factorial:
			std::cin >> problem_size;
			memory = program_naive_factorial(problem_size);
			escape_point = naive_factorial_exit_pc();
			break;
		case quad_naive_factorial:
			std::cin >> problem_size;
			memory = program_quad_naive_factorial(problem_size);
			escape_point = naive_quad_factorial_exit_pc();
			break;
		case bubble_sort:
			std::cin >> problem_size;
			for (size_t i = 0; i < problem_size; i++)
			{
				std::cin >> int_buffer;
				a.push_back(int_buffer);
			}
			memory = program_bubble_sort(a);
			escape_point = naive_bubble_sort_exit_pc();
			break;
		default:
			std::cout << "Error, invalid input\n";
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			return 0;
		}
		std::cin.clear();
		std::cin.ignore(10000, '\n');

		processor_pipelined proc{ memory };

		while (proc.return_register_value(register_names::program_counter) != escape_point)
		{
			proc.clock_tick();
			if (interactive) { interrogate_processor(proc, program, problem_size, false); }
		}
		std::cout << "Program finished\n";
		interrogate_processor(proc, program, problem_size, true);
	} while (1);
  //  std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
