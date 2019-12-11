#pragma once

#include "Instructions.h"
#include "register_lock_file.h"
#include "register_vector.h"
class writeback_unit
{
public:
	void receive_issue(instructions opcode, int64_t instruction_pc, int64_t destination, int64_t op1, int64_t op2, int64_t accumulator);
	bool is_ready();
	void clock_tick();
	void forward();
	bool op_completed();
	bool writeback(std::vector<physical_register> &registers, register_lock_file& rlf);
	int64_t return_idle_ops();
	int64_t return_ops_performed();
private:

	bool is_ready_flag = true;
	bool op_complete_flag = false;
	int64_t result = 0;
	int64_t next_pc = 0;
	int64_t operations_performed = 0;
	int64_t idle_time = 0;
	register_vector accumulator_buffer{ 32,std::vector<int64_t>{} };
	register_vector pc_buffer{ 32,std::vector<int64_t>{} };
	register_vector opcode_buffer{ 32,std::vector<int64_t>{} };
	register_vector destination_buffer{ 32,std::vector<int64_t>{} };
};
