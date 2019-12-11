#pragma once

#include "Instructions.h"
class ALU
{
public:
	void receive_issue(int64_t opcode, int64_t destination, int64_t op1, int64_t op2,int64_t ROB_index);
	bool is_ready();
	void clock_tick();

	bool execute_instruction();
	int64_t get_result();
	int64_t get_ROB();
	void flush(int64_t index_start, int64_t index_end, int64_t size);
	bool get_finished_flag();
private:
	bool finished_flag = false;
	bool flushing = false;
	int64_t result = 0;
	int64_t time_delay = 0;
	physical_register buffer_busy{ register_names::buffer_destination,0 };
	physical_register buffer_destination{ register_names::buffer_destination,0 };
	physical_register buffer_op1{ register_names::buffer_op1,0 };
	physical_register buffer_op2{ register_names::buffer_op2,0 };
	physical_register buffer_opcode{ register_names::buffer_opcode,0 };
	physical_register buffer_ROB{ register_names::buffer_opcode,0 };
};
