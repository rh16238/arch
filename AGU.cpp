#include "AGU.h"

void AGU::receive_issue(int64_t opcode, int64_t destination, int64_t op1, int64_t op2, int64_t ROB_index)
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
	case instructions::STO:
	case instructions::STOR:
		time_delay += 2;
		break;
	case instructions::LDR:
	case instructions::LDRR:
		time_delay += 2;
		break;
	}
}

bool AGU::is_ready()
{
	return ((!flushing) && (buffer_busy.get_register_value() != 1 || finished_flag));
}

void AGU::clock_tick()
{
	buffer_opcode.clock_tick();
	buffer_destination.clock_tick();
	buffer_op1.clock_tick();
	buffer_op2.clock_tick();
	buffer_ROB.clock_tick();
	buffer_busy.clock_tick();
	time_delay = (time_delay) ? --time_delay : 0;
	flushing = false;
	finished_flag = false;
}

bool AGU::execute_instruction(std::vector<int64_t>& memory, Memory_Order_Buffer& MO)
{
	if (buffer_busy.get_register_value() != 1 || time_delay != 0 || flushing)
	{
		return false;
	}
	int64_t r1 = buffer_op1.get_register_value();
	int64_t r2 = buffer_op2.get_register_value();
	int64_t addr = 0;
	switch (buffer_opcode.get_register_value())
	{
	case instructions::STO:
	case instructions::STOR:
		addr = buffer_destination.get_register_value() + r2;
		MO.update_op(addr, buffer_op1.get_register_value(), buffer_opcode.get_register_value(), buffer_ROB.get_register_value());
		buffer_busy.set_register_value(0);
		finished_flag = true;
		return false;

		//memory[addr] = buffer_op1.get_register_value();
		//result = 0;
		//buffer_busy.set_register_value(0);
		//finished_flag = true;
		//return true;
		break;
	case instructions::LDR:
	case instructions::LDRR:
		addr = r1 + r2;
		MO.update_op(addr, -1, buffer_opcode.get_register_value(), buffer_ROB.get_register_value());
		buffer_busy.set_register_value(0);
		finished_flag = true;
		return false;

		//result = memory[addr];
		//buffer_busy.set_register_value(0);
		//finished_flag = true;
		//return true;
		break;
	}
	return false;

}

int64_t AGU::get_result()
{
	return result;
}

int64_t AGU::get_ROB()
{
	return buffer_ROB.get_register_value();
}

void AGU::flush(int64_t index_start, int64_t index_end, int64_t size)
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
			}
		}
		else
		{
			if (ROB_index <= index_end || index_start <= ROB_index)//loops over size index
			{
				buffer_busy.set_register_value(false);
				flushing = true;
			}
		}
	}
}

bool AGU::get_finished_flag()
{
	return finished_flag;
}
