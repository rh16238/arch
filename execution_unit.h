#pragma once

#include "Instructions.h"
#include "memory_access_unit.h"
class execution_unit
{
public:
	void receive_issue(instructions opcode, int64_t instruction_pc, int64_t destination, int64_t op1, int64_t op2);
	bool is_ready();
	void clock_tick();
	void forward(memory_access_unit& mau);
	bool op_completed();
	bool execute_instruction(std::vector<physical_register>& registers);
	int64_t get_result();
	int64_t get_destination();
	instructions get_instruction();
	int64_t get_instruction_pc();
	void flush(int64_t pc);
	bool requires_feedback();
	void set_feedback(bool val);
private:
	bool feedback = false;
	bool is_ready_flag = true;
	bool op_complete_flag = false;
	int64_t result = 0;
	int64_t time_delay = 0;
	physical_register buffer_destination{ register_names::buffer_destination,0 };
	physical_register buffer_op1{ register_names::buffer_op1,0 };
	physical_register buffer_op2{ register_names::buffer_op2,0 };
	physical_register buffer_opcode{ register_names::buffer_opcode,0 };
	physical_register buffer_pc{ register_names::buffer_opcode,0 };
};
