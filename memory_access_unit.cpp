
#include "memory_access_unit.h"
#include "writeback_unit.h"
void memory_access_unit::receive_issue(instructions opcode, int64_t instruction_pc, int64_t destination, int64_t op1, int64_t op2, int64_t accumulator_val)
{
	is_complete_flag = false;
	is_ready_flag = false;
	buffer_destination.set_register_value(destination);
	buffer_opcode.set_register_value(opcode);
	buffer_op1.set_register_value(op1);
	buffer_op2.set_register_value(op2);
	buffer_pc.set_register_value(instruction_pc);
	accumulator.set_register_value(accumulator_val);

	switch (opcode)
	{
	case NOP:
		break;
	case STO:
	case STOR:
		time_delay = 0;
		break;
	case LDR:
	case LDRR:
		time_delay = 0;
		break;
	case BRI:
	case BRR:
	case JMP:
	case JMPR:
	case ADDR:
	case ADDI:
	case MULR:
	case MULI:
	case CMPR:
	case CMPI:
	case LDC:
		break;
	default:
		break;
	}
}

bool memory_access_unit::is_ready()
{
	return is_ready_flag;
}

bool memory_access_unit::is_complete()
{
	return is_complete_flag;
}

void memory_access_unit::clock_tick()
{
	if (time_delay > 0){--time_delay;}
	buffer_destination.clock_tick();
	buffer_opcode.clock_tick();
	buffer_op1.clock_tick();
	buffer_op2.clock_tick();
	accumulator.clock_tick();
	buffer_pc.clock_tick();
}

bool memory_access_unit::forward(writeback_unit& wb)
{

	if (is_complete_flag)
	{
		wb.receive_issue(
			static_cast<instructions>(buffer_opcode.get_register_value()),
			buffer_pc.get_register_value(),
			buffer_destination.get_register_value(),
			buffer_op1.get_register_value(),
			buffer_op2.get_register_value(),
			result);
		is_ready_flag = true;
		is_complete_flag = false;
	}
	else
	{
		wb.receive_issue(NOP,0, 0, 0, 0, 0);
	}


	return false;
}

bool memory_access_unit::mem_access(std::vector<int64_t>& memory, std::vector<physical_register>& registers,bool halt_mem)
{
	if (time_delay == 0 && !is_complete_flag)
	{

		switch (buffer_opcode.get_register_value())
		{

		case STO:
		case STOR:
				registers[status].set_register_value(memory_write + memory_wait);
				memory[accumulator.get_register_value()] = buffer_op1.get_register_value();
				result = accumulator.get_register_value();
				is_complete_flag = true;
				return true;

			break;
		case LDR:
		case LDRR:

				result = memory[accumulator.get_register_value()];
				registers[status].set_register_value(memory_read + memory_wait);
				accumulator.set_register_value(result);
				is_complete_flag = true;
				return true;

			break;
		case NOP:
		case BRI:
		case BRR:
		case JMP:
		case JMPR:
		case ADDR:
		case ADDI:
		case MULR:
		case MULI:
		case CMPR:
		case CMPI:
		case LDC:
			is_complete_flag = true;
			result = accumulator.get_register_value();
			break;
		default:
			break;
		}

	}
	return false;
}
