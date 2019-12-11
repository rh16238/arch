#pragma once
#include "math_utils.h"
#include "Register.h"
#include "mem_hierarchy.h"
#include <vector>
#include "Instructions.h"
class processor_basic
{
public:
	void clock_tick();
	processor_basic(const std::vector<int64_t> memory_stat );
	int64_t return_register_value(register_names reg);
	void print(std::ostream& os);
	const std::vector<int64_t>& return_memory();
	int return_idle_time();
	int return_executed_instructions();
	int return_clock_time();
private:
	int idle_time=0;
	int executed_instructions=0;
	int clock_ticks=0;
	void fetch_instruction();
	void execute_instruction(instruction i);
	void decode_instruction(instruction i);
	void memory_access_instruction(instruction x);
	void writeback_instruction(instruction x);

	std::vector<physical_register> registers{
	physical_register{ register_names::status,0lu },
	physical_register{ register_names::program_counter,0lu },
	physical_register{ register_names::memory_buffer,0lu } ,
	physical_register{ register_names::memory_address,0lu },
	physical_register{ register_names::instruction_register,0lu },
	physical_register{ register_names::r0,0lu },
	physical_register{ register_names::r1,0lu },
	physical_register{ register_names::r2,0lu },
	physical_register{ register_names::r3,0lu },
	physical_register{ register_names::r4,0lu },
	physical_register{ register_names::r5,0lu },
	physical_register{ register_names::r6,0lu },
	physical_register{ register_names::r7,0lu },
	physical_register{ register_names::r8,0lu },
	physical_register{ register_names::r9,0lu },
	physical_register{ register_names::r10,0lu},
	physical_register{ register_names::r11,0lu },
	physical_register{ register_names::r12,0lu },
	physical_register{ register_names::r13,0lu },
	physical_register{ register_names::r14,0lu },
	physical_register{ register_names::r15,0lu },
	physical_register{ register_names::accumulator,0lu }, 
	physical_register{ register_names::buffer_opcode,0lu },
	physical_register{ register_names::buffer_destination,0lu }, 
	physical_register{ register_names::buffer_op1,0lu }, 
	physical_register{ register_names::buffer_op2,0lu }, };


	memory_hierarchy memory;
	const std::vector<int64_t> test_memory;
	unsigned int stage = 0;
	const unsigned int fetch_stage = 0;
	const unsigned int decode_stage = 1;
	const unsigned int execute_stage = 2;
	const unsigned int memory_access = 3;
	const unsigned int write_back = 4;
};