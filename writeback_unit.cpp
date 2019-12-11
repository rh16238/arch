
#include "writeback_unit.h"
#include "register_lock_file.h"
void writeback_unit::receive_issue(instructions opcode, int64_t instruction_pc, int64_t destination, int64_t op1, int64_t op2,int64_t accumulator_val)
{
	if (opcode == 0 && instruction_pc == 0)
	{
		return;//If System NOP
	}
	op_complete_flag = false;
	is_ready_flag = false;

	pc_buffer.add_item(instruction_pc);
	accumulator_buffer.add_item(accumulator_val);
	opcode_buffer.add_item(opcode);
	destination_buffer.add_item(destination);
}
int64_t writeback_unit::return_idle_ops()
{
	return idle_time;
}
int64_t writeback_unit::return_ops_performed()
{
	return operations_performed;
}
bool writeback_unit::is_ready()
{
	return pc_buffer.next_length() != pc_buffer.get_max_size();
}
bool writeback_unit::op_completed()
{
	return op_complete_flag;
}
bool writeback_unit::writeback(std::vector<physical_register>& registers, register_lock_file& rlf)
{

	for (int64_t i = 0; i < opcode_buffer.avail_length(); i++)
	{
		auto buffer_opcode = opcode_buffer.get_item_at_index(i);
		auto buffer_pc = pc_buffer.get_item_at_index(i);
		auto buffer_accumulator = accumulator_buffer.get_item_at_index(i);
		auto buffer_destination = destination_buffer.get_item_at_index(i);
		if ((buffer_opcode == 0 && buffer_pc == 0))
		{
		 idle_time++;
		 opcode_buffer.remove_item_at_index(i);
		 pc_buffer.remove_item_at_index(i);
		 accumulator_buffer.remove_item_at_index(i);
		 destination_buffer.remove_item_at_index(i);
		 continue;
		}
		if (buffer_pc != next_pc)
		{
			continue;
		}
		++operations_performed;
		++next_pc;
		registers[program_counter].set_register_value(next_pc);
		uint64_t t = buffer_opcode;
		switch (t)
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
			registers[buffer_destination].set_register_value(buffer_accumulator);
			break;
		case LDR:
		case LDRR:
			registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() & ~(memory_op_fin^memory_wait^memory_read^memory_write));
			//registers[buffer_destination].set_register_value(registers[memory_buffer].get_register_value());
			registers[buffer_destination].set_register_value(buffer_accumulator);
			break;
		case BRI:
		case BRR:
		case JMP:
		case JMPR:
			registers[program_counter].set_register_value(buffer_accumulator);
			if (next_pc != buffer_accumulator)
			{

				for (int64_t i = 0; i < destination_buffer.avail_length(); i++)
				{
					auto opcode_at_index = opcode_buffer.get_item_at_index(i);
					switch (opcode_at_index)
					{
					case instructions::NOP://Ignore dest, op1 and op2.
					case instructions::STOR:
					case instructions::STO:
						break;
					case instructions::BRR:
					case instructions::BRI:
					case instructions::JMPR:
					case instructions::JMP:
						rlf.unlock_reg(program_counter);
						break;
					case instructions::ADDR://all ops set as needed
					case instructions::MULR://all ops set as needed
					case instructions::CMPR://all ops set as needed
					case instructions::LDRR://all ops set as needed
					case instructions::ADDI:
					case instructions::MULI:
					case instructions::CMPI:
					case instructions::LDR:
					case instructions::LDC:
						rlf.unlock_reg(destination_buffer.get_item_at_index(i));

						break;
					default:
						break;
					}


				}
				pc_buffer.flush();
				accumulator_buffer.flush();
				opcode_buffer.flush();
				destination_buffer.flush();

			}
			next_pc = buffer_accumulator;
			rlf.unlock_reg(program_counter);
			return true;
			break;
		default:
			break;
		}

		switch (t)
		{
		case instructions::NOP://Ignore dest, op1 and op2.
		case instructions::STOR:
		case instructions::STO:
		case instructions::BRR:
		case instructions::BRI:
		case instructions::JMPR:
		case instructions::JMP:
			break;
		case instructions::ADDR:
		case instructions::MULR:
		case instructions::CMPR:
		case instructions::LDRR:
		case instructions::ADDI:
		case instructions::MULI:
		case instructions::CMPI:
		case instructions::LDR:
		case instructions::LDC:
			if (rlf.is_reg_locked(buffer_destination) == buffer_pc)
			{
				rlf.unlock_reg(buffer_destination);
			}

			break;
		default:
			break;
		}
		opcode_buffer.remove_item_at_index(i);
		pc_buffer.remove_item_at_index(i);
		accumulator_buffer.remove_item_at_index(i);
		destination_buffer.remove_item_at_index(i);
		return true;
	}

	return false;


}
void writeback_unit::clock_tick()
{

	pc_buffer.clock_tick();
	accumulator_buffer.clock_tick();
	destination_buffer.clock_tick();
	opcode_buffer.clock_tick();
}

void writeback_unit::forward()
{
	is_ready_flag = true;
}
