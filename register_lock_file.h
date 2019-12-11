#pragma once

#include "Register.h"
class register_lock_file
{
public :
	int64_t is_reg_locked(int64_t reg);
	void lock_reg(int64_t reg, int64_t pc);
	void unlock_reg(int64_t reg);
	void clock_tick();
private:
	std::vector<physical_register> registers{
	physical_register{ register_names::status,-1l },
	physical_register{ register_names::program_counter,-1l },
	physical_register{ register_names::memory_buffer,-1l } ,
	physical_register{ register_names::memory_address,-1l },
	physical_register{ register_names::instruction_register,-1l },
	physical_register{ register_names::r0,-1l },
	physical_register{ register_names::r1,-1l },
	physical_register{ register_names::r2,-1l },
	physical_register{ register_names::r3,-1l },
	physical_register{ register_names::r4,-1l },
	physical_register{ register_names::r5,-1l },
	physical_register{ register_names::r6,-1l },
	physical_register{ register_names::r7,-1l },
	physical_register{ register_names::r8,-1l },
	physical_register{ register_names::r9,-1l },
	physical_register{ register_names::r10,-1l},
	physical_register{ register_names::r11,-1l },
	physical_register{ register_names::r12,-1l },
	physical_register{ register_names::r13,-1l },
	physical_register{ register_names::r14,-1l },
	physical_register{ register_names::r15,-1l },
	physical_register{ register_names::accumulator,-1l },
	physical_register{ register_names::buffer_opcode,-1l },
	physical_register{ register_names::buffer_destination,-1l },
	physical_register{ register_names::buffer_op1,-1l },
	physical_register{ register_names::buffer_op2,-1l }, };
};
