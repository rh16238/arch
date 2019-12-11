
#include "Reorder_Buffer.h"

Reorder_Buffer::Reorder_Buffer(int64_t size)
{
	this->ROB_size = size;
	ROB = std::vector<ROB_entry>{ ( unsigned int) size,ROB_entry{register_names::buffer_op1,instructions::NOP,2l,false} };
	Next_ROB = ROB;
}

bool Reorder_Buffer::is_ready()
{
	return ROB_next_avail != -1;
}

void Reorder_Buffer::common_data_bus(int64_t ROB, int64_t value)
{
	Next_ROB[ROB].value = value;
	Next_ROB[ROB].value_set = true;
}

Reorder_Buffer::ROB_Flush_info Reorder_Buffer::flush_on(int64_t ROB, Register_Alias_Table& RAT)//-1 //4
{
	ROB_Flush_info return_val{};
	return_val.ROB_size = ROB_size;

	//return_val.Stop_Index = (ROB_next_avail != -1) ? ROB_next_avail : (ROB_next_start == 0 ? ROB_size - 1 : ROB_next_start - 1);
	if (ROB_next_avail != -1)
	{
		return_val.Stop_Index = ROB_next_avail;
	}
	else
	{
		if (ROB_next_start == 0)
		{
			return_val.Stop_Index = ROB_size - 1;
		}
		else
		{
			return_val.Stop_Index = ROB_next_start - 1;
		}
	}

	ROB_next_avail = (ROB + 1 == ROB_size) ? 0 : ROB + 1;

	return_val.Start_Index = ROB_next_avail;
	if (ROB_next_avail == ROB_next_start)
	{
		ROB_next_avail = -1;
		return_val.Start_Index = -1;
		return_val.Stop_Index = -1;
		return return_val;
	}
	for (size_t i = return_val.Start_Index; i != return_val.Stop_Index; i = (i+1) % ROB_size)
	{
		Next_ROB[i].value_set = false;
		RAT.free_references(i);
	}
	RAT.free_references(return_val.Stop_Index);
	Next_ROB[return_val.Stop_Index].value_set = false;
	return return_val;
}

int64_t Reorder_Buffer::reserve_ROB_index(int64_t destination, int64_t opcode)
{
	if (ROB_next_avail != -1)
	{
		return add_entry(Next_ROB, ROB_next_start, ROB_next_avail, destination, opcode);
	}
	return -1;
}

void Reorder_Buffer::write_back(int entries_to_write, std::vector<physical_register>& registers, Register_Alias_Table& RAT, Memory_Order_Buffer& MO)
{
	for (size_t i = 0; i < entries_to_write; i++)
	{
		if (!write_ROB(registers, RAT,MO))
		{
			return;
		}
	}
}

void Reorder_Buffer::clock_tick()
{
	ROB = Next_ROB;
	ROB_avail=ROB_next_avail;
	ROB_start = ROB_next_start;
}

bool Reorder_Buffer::get_ROB_value_written(int64_t ROB_index)
{
	return ROB[ROB_index].value_set;
}

int64_t Reorder_Buffer::get_ROB_value(int64_t ROB_index)
{
	return ROB[ROB_index].value;
}
int64_t Reorder_Buffer::get_ops_performed()
{
	return ops_performed;
}

int64_t Reorder_Buffer::get_idle_ops_performed()
{
	return idle_time;
}

void Reorder_Buffer::test_flush(Memory_Order_Buffer & MO)
{
	for (size_t i = ROB_next_start; i != ROB_next_avail; i = (i + 1) % ROB_size)
	{
		auto& x = Next_ROB[i];
		switch (x.opcode)
		{
		case LDRR:
		case LDR:
		case STO:
		case STOR:
			MO.add_op(i, x.opcode);
			break;
		default:
			break;
		}
	}
}

int64_t Reorder_Buffer::add_entry(std::vector<ROB_entry>& ROB, int64_t & start_index, int64_t & next_index, int64_t dest, int64_t opcode )
{
	int64_t return_value = next_index;
	ROB[next_index].dest = dest;
	ROB[next_index].opcode = opcode;
	ROB[next_index].value_set = false;

	next_index = (next_index==ROB_size-1) ? 0 : next_index + 1;
	next_index = (next_index == start_index) ? -1 : next_index;

	return return_value;

}

void Reorder_Buffer::delete_entry(std::vector<ROB_entry>& ROB, int64_t & start_index, int64_t & next_index)
{
	if (next_index == -1)
	{
		next_index = start_index;
	}
	start_index = (start_index == ROB_size - 1) ? 0 : start_index + 1;
}

bool Reorder_Buffer::write_ROB(std::vector<physical_register>& registers, Register_Alias_Table& RAT, Memory_Order_Buffer& MO)//make sure to writeback before exec finishes.
{

	if (ROB_next_start == ROB_next_avail)
	{
		return false;
	}
	auto ROB_index = ROB_next_start;
	ROB_entry value_to_write = ROB[ROB_next_start];
	if ((value_to_write.opcode == STO || value_to_write.opcode == STOR)&& value_to_write.value_set ==false)
	{
		MO.allow_write_graduate(ROB_index);
		return true;
	}

	if (!value_to_write.value_set)
	{
		return false;
	}
	Next_ROB[ROB_next_start].value_set = false;
	++ops_performed;
	++next_pc;
	delete_entry(Next_ROB, ROB_next_start, ROB_next_avail);
		switch (value_to_write.opcode)
		{
		case NOP:
			break;
		case STO:
		case STOR:
			registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() & ~(memory_op_fin^memory_wait^memory_read^memory_write));
			break;
		case ADDR:
		case ADDI:
		case MULR:
		case MULI:
		case CMPR:
		case CMPI:
		case LDC:
			registers[value_to_write.dest].set_register_value(value_to_write.value);
			RAT.free_register(value_to_write.dest, ROB_index);
			break;
		case LDR:
		case LDRR:
			registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() & ~(memory_op_fin^memory_wait^memory_read^memory_write));
			//registers[value_to_write.dest].set_register_value(registers[memory_buffer].get_register_value());
			registers[value_to_write.dest].set_register_value(value_to_write.value);
			RAT.free_register(value_to_write.dest, ROB_index);
			break;
		case BRI:
		case BRR:
		case JMP:
		case JMPR:
			if (value_to_write.value != -1)
			{
				//registers[program_counter].set_register_value(value_to_write.value);
				next_pc = value_to_write.value;
			}
			break;
		default:
			break;
		}
		registers[program_counter].set_register_value(next_pc);

		return true;



}
