
#include "decode_unit.h"
#include "fetch_unit.h"
#include <algorithm>
bool fetch_unit::is_ready()
{
	return is_ready_flag;
}
bool fetch_unit::op_completed()
{
	return op_complete_flag;
}
bool fetch_unit::fetch_instruction(std::vector<int64_t>&test_memory, std::vector<physical_register> &registers, Branch_Predictor& BP, int64_t instruction_to_load)
{

	if (time_delay == 0)
	{
		if (predicted_pc < static_cast<int64_t>(test_memory.size()))
		{
			const auto instructions_to_load = std::min(instruction_buffer.get_max_size() - instruction_buffer.avail_length(), instruction_to_load);
			for (auto i = 0; i < instructions_to_load; i++)
			{

				if (static_cast<int64_t>(test_memory.size()) <= predicted_pc)
				{
					instruction_buffer.add_item(0);

				}
				else
				{
					instruction_buffer.add_item(test_memory[predicted_pc]);
				}
				pc_buffer.add_item(predicted_pc);

				auto next_pc = BP.determine_next_pc(predicted_pc);
				bool seperated_loads = (next_pc != predicted_pc + 1);
				predicted_pc = next_pc;
				predicted_pc_buffer.add_item(predicted_pc);
				if (seperated_loads) { break; }
			}

		op_complete_flag = true;
		}
	}
	return true;
}

void fetch_unit::flush_pipeline(uint64_t program_counter_temp)
{
	is_ready_flag = true;
	op_complete_flag = false;
	predicted_pc = program_counter_temp;
	instruction_buffer.flush();
	pc_buffer.flush();
	predicted_pc_buffer.flush();
}


int64_t fetch_unit::return_instruction(Instruction_Issue_Unit& iiu,int64_t number_to_forward )
{
	auto foward_number = std::min(std::min(iiu.get_available_space(), number_to_forward),instruction_buffer.avail_length());
	for (auto i = 0; i < foward_number; i++)
	{
		iiu.receive_issue(instruction_buffer.get_item_at_index(i), pc_buffer.get_item_at_index(i), predicted_pc_buffer.get_item_at_index(i));
	}
	for (auto i = 0; i < foward_number; i++)
	{
		instruction_buffer.remove_item_at_index(0);
		pc_buffer.remove_item_at_index(0);
		predicted_pc_buffer.remove_item_at_index(0);
	}
	return foward_number;
}

void fetch_unit::clock_tick()
{
	if (time_delay > 0) { --time_delay; }
	instruction_buffer.clock_tick();
	pc_buffer.clock_tick();
	predicted_pc_buffer.clock_tick();
}
