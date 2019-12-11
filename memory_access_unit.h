#pragma once

#include "Instructions.h"
#include "writeback_unit.h"
class memory_access_unit
{
public:
	void receive_issue(instructions opcode, int64_t instruction_pc, int64_t destination, int64_t op1, int64_t op2, int64_t accumulator);
	bool is_ready();
	bool is_complete();
	void clock_tick();
	bool forward(writeback_unit& wb);
	bool mem_access(std::vector<int64_t>& memory, std::vector<physical_register>& registers, bool halt_mem);
private:
	bool is_ready_flag = true;
	bool is_complete_flag = false;
	int64_t result = 0;
	int64_t time_delay = 0;
	physical_register accumulator{ register_names::accumulator,0 };
	physical_register buffer_destination{ register_names::buffer_destination,0 };
	physical_register buffer_op1{ register_names::buffer_op1,0 };
	physical_register buffer_op2{ register_names::buffer_op2,0 };
	physical_register buffer_opcode{ register_names::buffer_opcode,0 };
	physical_register buffer_pc{ register_names::program_counter,0 };
};
