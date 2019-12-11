#pragma once
#include "math_utils.h"
enum status_register_flags
{
	memory_write = 1,
	memory_read = 2,
	memory_wait = 4,
	memory_op_fin = 8,
};
enum register_names
{
	status,
	program_counter,
	memory_buffer,
	memory_address,
	instruction_register,
	r0,
	r1,
	r2,
	r3,
	r4,
	r5,
	r6,
	r7,
	r8,
	r9,
	r10,
	r11,
	r12,
	r13,
	r14,
	r15,
	accumulator,
	buffer_opcode,
	buffer_destination,
	buffer_op1,
	buffer_op2,
	LAST_ITEM,
};

class physical_register
{
	private:
		const register_names name_of_register;
		int64_t working_register_value;
		int64_t available_register_value;
	public:
		physical_register(const register_names name, const int64_t initial_value);
		const register_names get_register_name();
		int64_t get_register_value();
		void set_register_value(int64_t value);
		void clock_tick();
		void print(std::ostream& os);

		const static char*  register_name_to_string(const register_names register_name);
	
		
};