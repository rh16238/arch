#pragma once

#include "Instructions.h"
#include "execution_unit.h"
#include "register_lock_file.h"
#include "register_vector.h"
class decode_unit
{
public:
	void receive_issue(int64_t instruction, int64_t pc_val);
	bool is_ready();
	void clock_tick();
	void forward(execution_unit& mau, register_lock_file& rlf);
	bool op_completed(int64_t index);
	bool decode_instructions(std::vector<physical_register>& registers, register_lock_file& rlf);
	void flush_pipeline();
	void replace_register_val(int64_t reg, int64_t val, register_lock_file& rlf);
private:
	bool op1_decoded = false;
	bool op2_decoded = false;
	bool destination_decoded = false;
	bool is_ready_flag = true;

	int64_t result = 0;
	int64_t result_pc = 0;
	int64_t time_delay = 0;
	physical_register buffer_instruction{ instruction_register, 0 };
	physical_register buffer_pc{ program_counter, 0 };
	uint64_t buffer_destination = 0;
	uint64_t buffer_op1 = 0;
	uint64_t buffer_op2 = 0;
	uint64_t buffer_opcode = 0;
	register_vector instruction_buffer_v{ 16,std::vector<int64_t>{} };
	register_vector pc_buffer_v{ 16,std::vector<int64_t>{} };
	std::vector<uint64_t> destination_buffer;
	std::vector<uint64_t> op1_buffer;
	std::vector<uint64_t> op2_buffer;
	std::vector<uint64_t> opcode_buffer;
	std::vector<bool> op1_decoded_buffer;
	std::vector<bool> op2_decoded_buffer;
	std::vector<bool> destination_decoded_buffer;
	bool branch_issued = false;

};
