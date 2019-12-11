#pragma once

#include "Instructions.h"
class Branching_Unit
{
public:
	void receive_issue(int64_t opcode, int64_t destination, int64_t op1, int64_t op2, int64_t ROB_index, int64_t PC, int64_t predicted_pc);
	bool is_ready();
	void clock_tick();

	bool execute_instruction();
	int64_t get_pc();
	int64_t get_predicted_pc();
	int64_t get_result();
	int64_t get_ROB();
	bool get_predictive_error();
	void flush(int64_t index_start, int64_t index_end, int64_t size);
private:
	bool predictive_error = false;
	bool flushing = false;
	int64_t result = 0;
	int64_t time_delay = 0;
	physical_register buffer_busy{ register_names::buffer_destination,0 };
	physical_register buffer_destination{ register_names::buffer_destination,0 };
	physical_register buffer_op1{ register_names::buffer_op1,0 };
	physical_register buffer_op2{ register_names::buffer_op2,0 };
	physical_register buffer_opcode{ register_names::buffer_opcode,0 };
	physical_register buffer_ROB{ register_names::buffer_opcode,0 };

	physical_register buffer_PC{ register_names::buffer_opcode,0 };
	physical_register buffer_Predicted_PC{ register_names::buffer_opcode,0 };
};
