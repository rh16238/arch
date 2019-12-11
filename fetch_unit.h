#pragma once

#include "Instructions.h"
#include "decode_unit.h"
#include "register_vector.h"
#include "Instruction_Issue_Unit.h"
#include "branch_predictor.h"
class fetch_unit
{
public:
	bool is_ready();
	void clock_tick();
	bool op_completed();
	bool fetch_instruction(std::vector<int64_t>&test_memory, std::vector<physical_register> &registers, Branch_Predictor& BP, int64_t instruction_to_load);
	void flush_pipeline(uint64_t program_counter_temp);
	int64_t return_instruction(Instruction_Issue_Unit& iiu, int64_t number_to_forward);
private:
	int64_t predicted_pc = 0;
	bool is_ready_flag = true;
	bool op_complete_flag = false;
	int64_t time_delay = 0;
	register_vector instruction_buffer{ 8,std::vector<int64_t>{} };
	register_vector pc_buffer{ 8,std::vector<int64_t>{} };
	register_vector predicted_pc_buffer{ 8,std::vector<int64_t>{} };
};
