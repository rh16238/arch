
#include "Register.h"
#include "math_utils.h"

physical_register::physical_register(const register_names name,const int64_t initial_value) : name_of_register(name),working_register_value(initial_value),available_register_value(initial_value)
{
}

const register_names physical_register::get_register_name()
{
	return name_of_register;
}

int64_t physical_register::get_register_value()
{
	return available_register_value;
}

void physical_register::set_register_value(int64_t value)
{
	working_register_value = value;
}

void physical_register::clock_tick()
{
	available_register_value = working_register_value;
}


void physical_register::print(std::ostream& os)
{
	os << register_name_to_string(name_of_register) << " interim: ";
	if (working_register_value)
	{
		os << working_register_value;
	}
	else
	{
		os << "NA";
	}
	os << " output ";
	if (available_register_value)
	{
		 os << available_register_value<<'\n';
		return;
	}
		os  << "NA\n";

}

const char * physical_register::register_name_to_string(const register_names register_name)
{

		switch (register_name)
		{//status
		case register_names::status:
			return "status";
		case register_names::r0:
			return "r0";
		case register_names::r1:
			return "r1";
		case register_names::r2:
			return "r2";
		case register_names::r3:
			return "r3";
		case register_names::r4:
			return "r4";
		case register_names::r5:
			return "r5";
		case register_names::r6:
			return "r6";
		case register_names::r7:
			return "r7";
		case register_names::r8:
			return "r8";
		case register_names::r9:
			return "r9";
		case register_names::r10:
			return "r10";
		case register_names::r11:
			return "r11";
		case register_names::r12:
			return "r12";
		case register_names::r13:
			return "r13";
		case register_names::r14:
			return "r14";
		case register_names::r15:
			return "r15";
		case register_names::program_counter:
			return "program counter";
		case register_names::instruction_register:
			return "instruction_register";
		case register_names::memory_address:
			return "memory address";
		case register_names::memory_buffer:
			return "memory buffer";
		case register_names::accumulator:
			return "accumulator";
		case register_names::buffer_opcode:
			return "buffer opcode";
		case register_names::buffer_destination:
			return "buffer destination";
		case register_names::buffer_op1:
			return "buffer op1";
		case register_names::buffer_op2:
			return "buffer op2";
		default:
			return "ERROR UNKNOWN REGISTER";
		}
}
