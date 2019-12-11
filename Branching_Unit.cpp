
#include "Branching_Unit.h"


void Branching_Unit::receive_issue(int64_t opcode, int64_t destination, int64_t op1, int64_t op2, int64_t ROB_index, int64_t PC, int64_t predicted_pc)
{
	buffer_opcode.set_register_value(opcode);
	buffer_destination.set_register_value(destination);
	buffer_op1.set_register_value(op1);
	buffer_op2.set_register_value(op2);
	buffer_ROB.set_register_value(ROB_index);

	buffer_PC.set_register_value(PC);
	buffer_Predicted_PC.set_register_value(predicted_pc);

	buffer_busy.set_register_value(1);
	time_delay = 1;
	switch (buffer_opcode.get_register_value())
	{
	case instructions::BRI:
	case instructions::BRR:
		time_delay += 2;
		break;
	case instructions::JMP:
		time_delay += 0;
		break;
	case instructions::JMPR:
		time_delay += 2;
		break;
	}
	time_delay = 0;
}
bool Branching_Unit::is_ready()
{
	return ((!flushing) && (buffer_busy.get_register_value() != 1 ));
}

void Branching_Unit::clock_tick()
{
	buffer_opcode.clock_tick();
	buffer_destination.clock_tick();
	buffer_op1.clock_tick();
	buffer_op2.clock_tick();
	buffer_ROB.clock_tick();
	buffer_busy.clock_tick();
	buffer_PC.clock_tick();
	buffer_Predicted_PC.clock_tick();
	time_delay = (time_delay) ? --time_delay : 0;
	flushing = false;
}

bool Branching_Unit::execute_instruction()
{
	if (buffer_busy.get_register_value() != 1 || time_delay != 0 || flushing)
	{
		return false;
	}
	int64_t r1 = buffer_op1.get_register_value();
	int64_t r2 = buffer_op2.get_register_value();
	bool branch_taken = false;
	switch (buffer_opcode.get_register_value())
	{
	case instructions::BRI:
	case instructions::BRR:
		branch_taken = (r1 == r2);
		result = (r1 == r2) ? buffer_destination.get_register_value() : buffer_PC.get_register_value()+1;
		//predictive_error = branch_taken;
		predictive_error = (result != buffer_Predicted_PC.get_register_value());
		if (branch_taken != predictive_error)
		{
			//printf("Test");
			branch_taken = false;
		}
		break;
	case instructions::JMP:
		result = r1;
		predictive_error = (result != buffer_Predicted_PC.get_register_value());
		break;
	case instructions::JMPR:
		result = r1 + r2;
		predictive_error = (result != buffer_Predicted_PC.get_register_value());
		break;
	}
	buffer_busy.set_register_value(0);
	return true;
}

int64_t Branching_Unit::get_pc()
{
	return buffer_PC.get_register_value();
}

int64_t Branching_Unit::get_predicted_pc()
{
	return buffer_Predicted_PC.get_register_value();
}

int64_t Branching_Unit::get_result()
{
	return result;
}

int64_t Branching_Unit::get_ROB()
{
	return buffer_ROB.get_register_value();
}

bool Branching_Unit::get_predictive_error()
{
	return predictive_error;
}

void Branching_Unit::flush(int64_t index_start, int64_t index_end, int64_t size)
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
