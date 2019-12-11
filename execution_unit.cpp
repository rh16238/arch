
#include "execution_unit.h"
#include "memory_access_unit.h"
void execution_unit::receive_issue(instructions opcode,int64_t instruction_pc, int64_t destination, int64_t op1, int64_t op2)
{
	feedback = false;
	op_complete_flag = false;
	is_ready_flag = false;
	buffer_destination.set_register_value(destination);
	buffer_opcode.set_register_value(opcode);
	buffer_op1.set_register_value(op1);
	buffer_op2.set_register_value(op2);
	buffer_pc.set_register_value(instruction_pc);
	switch (opcode)
	{
	case NOP:
	case ADDR:
	case ADDI:
	case CMPR:
	case CMPI:
	case LDC:
	case BRI:
	case BRR:
	case JMP:
	case JMPR:
		time_delay = 0;
		break;
	case MULR:
	case MULI:
	case STO:
	case STOR:
	case LDR:
	case LDRR:
		time_delay = 0;
		break;
	default:
		time_delay = 0;
		break;
	}
}

bool execution_unit::is_ready()
{
	return is_ready_flag;
}
bool execution_unit::op_completed()
{
	return op_complete_flag;
}
bool execution_unit::execute_instruction(std::vector<physical_register>& registers)
{
	if (op_complete_flag|| time_delay !=0)
	{
		return false;
	}
	op_complete_flag = true;
	bool return_flag = false;
	int64_t r1 = buffer_op1.get_register_value();
	int64_t r2 = buffer_op2.get_register_value();
	feedback = true;
	switch (buffer_opcode.get_register_value())
	{
	case instructions::NOP:
		feedback = false;
		return false;
		break;
	case instructions::ADDR:
	case instructions::ADDI:
		result = r1 + r2;
		break;
	case instructions::MULR:
	case instructions::MULI:
		result = r1 * r2;
		break;
	case instructions::CMPR:
	case instructions::CMPI:
		result = r1 == r2 ? 0 : r1 > r2 ? 1 : -1;
		break;
	case instructions::LDR:
	case instructions::LDRR:
		result = r1 + r2;
		feedback = false;
		break;
	case instructions::LDC:
		result = r1;
		break;
	case instructions::BRI:
	case instructions::BRR:
		return_flag = (r1 == r2);
		result = (return_flag) ? buffer_destination.get_register_value() : buffer_pc.get_register_value()+1;
		feedback = false;
		break;
	case instructions::JMP:
		result = r1;
		return_flag = true;
		feedback = false;
		break;
	case instructions::JMPR:
		result = r1 + r2;
		return_flag = true;
		feedback = false;
		break;
	case instructions::STO:
	case instructions::STOR:
		result = buffer_destination.get_register_value() + r2;
		feedback = false;
		break;
	default:
		break;
	}
	return return_flag;
}
int64_t execution_unit::get_result()
{
	if (op_complete_flag)
	{
		return result;
	}
	return 0;
}
int64_t execution_unit::get_destination()
{
	if (op_complete_flag)
	{
		return buffer_destination.get_register_value();
	}
	return 10000;
}
instructions execution_unit::get_instruction()
{
	if (op_complete_flag)
	{
		return static_cast<instructions>(buffer_opcode.get_register_value());
	}
	return NOP;
}
int64_t execution_unit::get_instruction_pc()
{
	return buffer_pc.get_register_value();
}
void execution_unit::flush(int64_t pc)
{
	feedback = false;
	is_ready_flag = true;
	op_complete_flag = false;
}
bool execution_unit::requires_feedback()
{
	return feedback;
}
void execution_unit::set_feedback(bool val)
{
	feedback = val;
}
void execution_unit::clock_tick()
{
	if (time_delay > 0) { --time_delay; }
	buffer_destination.clock_tick();
	buffer_opcode.clock_tick();
	buffer_op1.clock_tick();
	buffer_op2.clock_tick();
	buffer_pc.clock_tick();
}

void execution_unit::forward(memory_access_unit& mau)
{
	if (op_complete_flag)
	{
		mau.receive_issue(
			static_cast<instructions>(buffer_opcode.get_register_value()),
			buffer_pc.get_register_value(),
			buffer_destination.get_register_value(),
			buffer_op1.get_register_value(),
			buffer_op2.get_register_value(),
			result);
		is_ready_flag = true;
		op_complete_flag = false;
		feedback = false;
	}
	else
	{
		mau.receive_issue(NOP,0, 0, 0, 0, 0);
	}
}
