
#include "ALU.h"

void ALU::receive_issue(int64_t opcode, int64_t destination, int64_t op1, int64_t op2, int64_t ROB_index)
{
	buffer_opcode.set_register_value(opcode);
	buffer_destination.set_register_value(destination);
	buffer_op1.set_register_value(op1);
	buffer_op2.set_register_value(op2);
	buffer_ROB.set_register_value(ROB_index);
	buffer_busy.set_register_value(1);
	time_delay = 1;
	switch (opcode)
	{
	case instructions::NOP:
		time_delay += 0;
		break;
	case instructions::ADDR:
	case instructions::ADDI:
		 time_delay += 2;
		break;
	case instructions::MULR:
	case instructions::MULI:
		time_delay += 4;
		break;
	case instructions::CMPR:
	case instructions::CMPI:
		time_delay += 2;
		break;
	case instructions::LDC:
		time_delay += 1;
		break;
	}
}

bool ALU::is_ready()
{
	return ((!flushing) && (buffer_busy.get_register_value() != 1 || finished_flag));

	//return (buffer_busy.get_register_value() != 1) || flushing;
}

void ALU::clock_tick()
{
	time_delay = (time_delay) ? --time_delay : 0;


	buffer_opcode.clock_tick();
	buffer_destination.clock_tick();
	buffer_op1.clock_tick();
	buffer_op2.clock_tick();
	buffer_ROB.clock_tick();
	buffer_busy.clock_tick();
	finished_flag = false;
	flushing = false;
}

bool ALU::execute_instruction()
{
	if (buffer_busy.get_register_value() != 1 || time_delay != 0||flushing)
	{
		return false;
	}
	int64_t r1 = buffer_op1.get_register_value();
	int64_t r2 = buffer_op2.get_register_value();
	switch (buffer_opcode.get_register_value())
	{
	case instructions::NOP:
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
	case instructions::LDC:
		result = r1;
		break;
	}
	buffer_busy.set_register_value(0);
	finished_flag = true;
	return true;
}

int64_t ALU::get_result()
{
	return result;
}

int64_t ALU::get_ROB()
{
	return buffer_ROB.get_register_value();
}

void ALU::flush(int64_t index_start, int64_t index_end, int64_t size)
{
	if (buffer_busy.get_register_value() == 1)
	{
		int64_t ROB_index = buffer_ROB.get_register_value();
		if (index_start <= index_end)
		{
			if (ROB_index <= index_end && index_start <= ROB_index)//ordinary check
			{
				buffer_busy.set_register_value(false);
				flushing = true;
				time_delay = 0;
			}
		}
		else
		{
			if (ROB_index <= index_end || index_start <= ROB_index)//loops over size index
			{
				buffer_busy.set_register_value(false);
				flushing = true;
				time_delay = 0;
			}
		}
	}
}

bool ALU::get_finished_flag()
{
	return finished_flag;
}
