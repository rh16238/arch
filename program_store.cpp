
#include "program_store.h"
#include "math_utils.h"
#include "Instructions.h"
#include "Registers.h"

std::vector<int64_t> program_rolled_test(int64_t ops_per_loop, int64_t loops, bool return_rolled)
{
	std::vector<int64_t> memory{};
	std::vector<instruction> program{};
	for (int i = 0; i < loops - 1; i++)
	{
		program.push_back(instruction{ instructions::NOP,r0,0 ,0 });
	}
	if (return_rolled)
	{
		program.push_back(instruction{ instructions::JMP,program_counter,0,0 });//else repeatedly nop

	}
	else
	{
		for (int i = 0; i < loops - 1; i++)
		{
			program.push_back(instruction{ instructions::NOP,r0,0 ,0 });
		}
	}
	for (auto& i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	return memory;
}
std::vector<int64_t> program_naive_factorial(int64_t factorial)
{
	std::vector<int64_t> memory{};
	std::vector<instruction> program{
		instruction{ instructions::LDC,r0,factorial,0},//loads factorial to count into memory
		instruction{ instructions::LDC,r1,factorial,0},//loads initial result
		instruction{ instructions::LDC,r15,7,0},//loads exit branch
		//calculate factorial
		//sub 1 from factorial
		instruction{ instructions::ADDI,r0,r0,-1},
		instruction{ instructions::MULR,r1,r0,r1},
		instruction{ instructions::BRI,r15,r0,2},//if counter was below value, return to branch location
		instruction{ instructions::JMP,program_counter,3,0 },//else repeatedly nop
		instruction{ instructions::NOP,r0,0 ,0},// do nothing
		instruction{ instructions::JMP,program_counter,7,0 },//else repeatedly nop
	};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	return memory;
}

int naive_factorial_exit_pc()
{
	return 8;
}

std::vector<int64_t> program_quad_naive_factorial(int64_t factorial)
{
	std::vector<int64_t> memory{};
	std::vector<instruction> program{

		instruction{ instructions::LDC,r0,factorial,0},//loads factorial to count into memory
		instruction{ instructions::LDC,r1,factorial,0},//loads initial result

		instruction{ instructions::LDC,r2,factorial,0},//loads factorial to count into memory
		instruction{ instructions::LDC,r3,factorial,0},//loads initial result

		instruction{ instructions::LDC,r4,factorial,0},//loads factorial to count into memory
		instruction{ instructions::LDC,r5,factorial,0},//loads initial result

		instruction{ instructions::LDC,r6,factorial,0},//loads factorial to count into memory
		instruction{ instructions::LDC,r7,factorial,0},//loads initial result
		instruction{ instructions::LDC,r15,19,0},//loads exit branch
		//calculate factorial
		//sub 1 from factorial
		instruction{ instructions::ADDI,r0,r0,-1},
		instruction{ instructions::ADDI,r2,r2,-1},
		instruction{ instructions::ADDI,r6,r6,-1},
		instruction{ instructions::ADDI,r4,r4,-1},

		instruction{ instructions::MULR,r3,r2,r3},//10
		instruction{ instructions::MULR,r5,r4,r5},//12
		instruction{ instructions::MULR,r7,r6,r7}, //16
		instruction{ instructions::MULR,r1,r0,r1},//8

		instruction{ instructions::BRI,r15,r2,2},//if counter was below value, return to branch location
		instruction{ instructions::JMP,program_counter,9,0 },//else repeatedly nop
		instruction{ instructions::NOP,r0,0 ,0},// do nothing
		instruction{ instructions::JMP,program_counter,19,0 },//else repeatedly nop
	};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	return memory;
}
int naive_quad_factorial_exit_pc()
{
	return 20;
}
std::vector<int64_t> program_vector_addition(std::vector<int64_t> a, std::vector<int64_t> b)
{
	int64_t size = a.size();
	if (size != b.size() || size < 1)
	{
		throw "Invalid vector sizes in program vector addition";
	}

	std::vector<instruction> program{
		instruction{ instructions::LDC,r11,17,0},//loads first memory address after program as address of C
		instruction{ instructions::LDR,r12,r11,0},//set to address of C
		instruction{ instructions::LDR,r13,r11,1},//set to address of A
		instruction{ instructions::LDR,r14,r11,2},//set to address of B
		instruction{ instructions::LDC,r10,5,0},//set branch location to below this line

		instruction{ instructions::LDRR,r2,r13,r1},//load value of A to r2
		instruction{ instructions::LDRR,r3,r14,r1},//load value of B to r3
		instruction{ instructions::ADDR,r3,r3,r2},//set r3 = A + B
		instruction{ instructions::STO,r12,r3,0},//store r3 in C
		instruction{ instructions::ADDI,r1,r1,1 },//increment counter

		instruction{ instructions::ADDI,r12,r12,1 },//increment counter
		instruction{ instructions::NOP,r0,0 ,0},// do nothing
		instruction{ instructions::NOP,r0,0 ,0},// do nothing
		instruction{ instructions::CMPI,r9,r1,size},//check counter
		instruction{ instructions::BRI,r10,r9,-1},//if counter was below value, return to branch location

		instruction{ instructions::NOP,r0,0 ,0},// do nothing
		instruction{ instructions::JMP,program_counter,15,0 },//else repeatedly nop
	};

	int64_t c_pos = 20;
	int64_t a_pos = c_pos + size;
	int64_t b_pos = a_pos + size ;
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	memory.push_back(c_pos);
	memory.push_back(a_pos);
	memory.push_back(b_pos);
	for (auto i = 0; i < size; i++)//push c
	{
		memory.push_back(0);
	}
	for (auto i = 0; i < size; i++)//push a
	{
		memory.push_back(a.at(i));
	}
	for (auto i = 0; i < size; i++)//push b
	{
		memory.push_back(b.at(i));
	}
	return memory;
}

int naive_vector_addition_exit_pc()
{
	return 16;
}


std::vector<int64_t> program_dot_product(std::vector<int64_t> a, std::vector<int64_t> b)
{
	int64_t size = a.size();
	if (size != b.size() || size < 1)
	{
		throw "Invalid vector sizes in program dot product";
	}

	std::vector<instruction> program{
		instruction{ instructions::LDC,r11,17,0},//loads first memory address after program as address of C
		instruction{ instructions::LDR,r12,r11,0},//set to address of C
		instruction{ instructions::LDR,r13,r11,1},//set to address of A
		instruction{ instructions::LDR,r14,r11,2},//set to address of B
		instruction{ instructions::LDC,r10,5,0},//set branch location to below this line

		instruction{ instructions::LDRR,r2,r13,r1},//load value of A to r2
		instruction{ instructions::LDRR,r3,r14,r1},//load value of B to r3
		instruction{ instructions::MULR,r3,r3,r2},//set r3 = A * B
		instruction{ instructions::ADDR,r5,r5,r3},//r5 = r5 + r3
		instruction{ instructions::ADDI,r1,r1,1 },//increment counter

		instruction{ instructions::ADDI,r12,r12,1 },//increment counter
		instruction{ instructions::NOP,r0,0 ,0},// do nothing
		instruction{ instructions::NOP,r0,0 ,0},// do nothing
		instruction{ instructions::CMPI,r9,r1,size},//check counter
		instruction{ instructions::BRI,r10,r9,-1},//if counter was below value, return to branch location

		instruction{ instructions::NOP,r0,0 ,0},// do nothing
		instruction{ instructions::JMP,program_counter,15,0 },//else repeatedly nop
	};

	int64_t c_pos = 20;
	int64_t a_pos = c_pos + size;
	int64_t b_pos = a_pos + size;
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	memory.push_back(c_pos);
	memory.push_back(a_pos);
	memory.push_back(b_pos);
	for (auto i = 0; i < size; i++)//push c
	{
		memory.push_back(0);
	}
	for (auto i = 0; i < size; i++)//push a
	{
		memory.push_back(a.at(i));
	}
	for (auto i = 0; i < size; i++)//push b
	{
		memory.push_back(b.at(i));
	}
	return memory;
}

int dot_product_exit_pc()
{
	return 16;
}

std::vector<int64_t> program_hazard_unroll(bool unroll)
{
	std::vector<int64_t> memory{};
	std::vector<instruction> program{
		instruction{instructions::LDC,r1,100,0},
		instruction{instructions::LDC,r10,1,0 },//load loop point
		instruction{ instructions::ADDI,r2,r1,2},
		instruction{ instructions::MULI,r3,r2,-1},
		instruction{ instructions::ADDI,r7,r7,1},

	};
	if (unroll)
	{
		for (size_t i = 0; i < 99; i++)
		{
			program.push_back(instruction{ instructions::ADDI,r2,r1,2 });
			program.push_back(instruction{ instructions::MULI,r3,r2,-1 });
			program.push_back(instruction{ instructions::ADDI,r7,r7,1 });
		}
	}
	else
	{
		program.push_back(instruction{ instructions::JMP,program_counter,2,0 });
	}

	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	return memory;
}
std::vector<int64_t> program_bubble_sort(std::vector<int64_t> a)
{
	int64_t size = a.size();
	if ( size < 2)
	{
		throw "Invalid vector sizes in bubblesort";
	}
	//load address of A
	//for int i = size-1;
	//load a
	//for auto y = size -i
	std::vector<instruction> program{
		//OUTER_LOOP
		instruction{ instructions::LDC,r6,24,0},//load pointer to swap function                                  0
		instruction{ instructions::LDC,r11,27,0},//loads first memory address after program as address of C      1
		instruction{ instructions::LDR,r11,r11,0},//loads first memory address after program as address of C     2
		instruction{ instructions::LDC,r12,size-1,0},//store number of array loops in r12                        3
		instruction{ instructions::LDC,r13,5,0},//set branch location to below this line                         4


		instruction{ instructions::JMP,register_names::program_counter,11,0},//set branch location to call inner 5
		instruction{ instructions::ADDI,r1,r1,1},//add 1 to loop;												 6
		instruction{ instructions::CMPR,r0,r1,r12},//r0 = cmp r1 (outer loop index) == number of array loops     7
		instruction{ instructions::BRI,r13,r0,-1},//Branch back into outer loop if r0 = -1;                      8
		instruction{ instructions::NOP,r0,0 ,0},// do nothing                                                    9

		instruction{ instructions::JMP,program_counter,9,0 },//else repeatedly nop ending pc =8                  10
		//INNER_LOOP
		instruction{ instructions::LDC,r2,0,0},//set index to 0                                                  11
		instruction{ instructions::MULI,r14,r1,-1},//store number of array loops in r12 = -numberof loops done +total
		instruction{ instructions::ADDR,r14,r14,r12},                                                          //13
		instruction{ instructions::LDC,r15,15,0},//set branch location to below this line //increase to 15       14

		instruction{ instructions::ADDR,r9,r11,r2},                                                            //15
		instruction{ instructions::LDR,r7,r9,0},//r7 = a														 16
		instruction{ instructions::LDR,r8,r9,1},//r8 = b                                                         17
		instruction{ instructions::CMPR,r0,r7,r8},//1 indicates incorrect order                                  18
		instruction{ instructions::BRI,r6,r0,1},//Branch to swap and store function if r1                        19

		instruction{ instructions::ADDI,r2,r2,1},//add 1 to loop;                                                20
		instruction{ instructions::CMPR,r0,r2,r14},//r0 = cmp r1 (inner loop index) == number of array loops     21
		instruction{ instructions::BRI,r15,r0,-1},//Branch back into inner loop if r0 = -1;                      22
		instruction{ instructions::JMP,register_names::program_counter,6,0},//set jump location to return to out 23
		//SWAP FUNCTION
		instruction{ instructions::STO,r9,r7,1},//r7 = a                                                         24
		instruction{ instructions::STO,r9,r8,0},//r8 = b                                                         25
		instruction{ instructions::JMP,register_names::program_counter,20,0},//set jump location to return to in 26

	};//16 first available memory

	int64_t c_pos = 28;
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	memory.push_back(c_pos);
	for (auto i : a)
	{
		memory.push_back(i);
	}

	return memory;
}


int naive_bubble_sort_exit_pc()
{
	return 10;
}
