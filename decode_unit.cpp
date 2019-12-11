
#include "decode_unit.h"
#include "execution_unit.h"
#include "register_lock_file.h"
#include <algorithm>
void decode_unit::receive_issue(int64_t instruction, int64_t pc_val)
{
	op1_decoded = false;
	op2_decoded = false;
	destination_decoded = false;
	is_ready_flag = false;
	buffer_instruction.set_register_value(instruction);
	buffer_pc.set_register_value(pc_val);

	instruction_buffer_v.add_item(instruction);
	pc_buffer_v.add_item(pc_val);
	op1_buffer.push_back(0);
	op2_buffer.push_back(0);
	destination_buffer.push_back(0);
	op1_decoded_buffer.push_back(false);
	op2_decoded_buffer.push_back(false);
	destination_decoded_buffer.push_back(false);
}
bool contains(std::vector<int64_t> vec, int64_t val)
{
	return (std::find(vec.begin(), vec.end(), val) != vec.end());
}

bool decode_unit::is_ready()
{
	return instruction_buffer_v.next_length() != instruction_buffer_v.get_max_size();
}
bool decode_unit::op_completed(int64_t index)
{
	auto op1_decoded = op1_decoded_buffer[index];
	auto op2_decoded = op2_decoded_buffer[index];
	auto destination_decoded = destination_decoded_buffer[index];
	return op1_decoded && op2_decoded && destination_decoded;
}
bool decode_unit::decode_instructions(std::vector<physical_register>& registers, register_lock_file& rlf)
{
	std::vector<int64_t> altered_destinations{};
	for (int64_t i = 0; i < instruction_buffer_v.avail_length(); i++)
	{
		if (op_completed(i))
		{
			altered_destinations.push_back(destination_buffer[i]);
			continue;
		}

		instruction x{ instruction_buffer_v.get_item_at_index(i) };

		buffer_opcode = x.get_opcode();
		auto op1_decoded = op1_decoded_buffer.at(i);
		auto op2_decoded = op2_decoded_buffer.at(i);
		auto destination_decoded = destination_decoded_buffer.at(i);
		auto buffer_op1 = op1_buffer.at(i);
		auto buffer_op2 = op2_buffer.at(i);
		auto buffer_destination = destination_buffer.at(i);

		switch (x.get_opcode())
		{
		case instructions::NOP://Ignore dest, op1 and op2.
			op1_decoded = true;
			op2_decoded = true;
			destination_decoded = true;
			break;
		case instructions::ADDR://all ops set as needed
		case instructions::MULR://all ops set as needed
		case instructions::CMPR://all ops set as needed
		case instructions::LDRR://all ops set as needed
			buffer_destination = x.get_destination();
			destination_decoded = true;
			if (!(op1_decoded || (rlf.is_reg_locked(x.get_operand_1())!=-1)) && !contains(altered_destinations,x.get_operand_1()))
			{
				buffer_op1 = registers[x.get_operand_1()].get_register_value();
				op1_decoded = true;
			}
			if (!(op2_decoded || (rlf.is_reg_locked(x.get_operand_2()) != -1)) && !contains(altered_destinations, x.get_operand_2()))
			{

				buffer_op2 = registers[x.get_operand_2()].get_register_value();
				op2_decoded = true;
			}

			break;
		case instructions::ADDI:
		case instructions::MULI:
		case instructions::CMPI:
		case instructions::LDR:
			buffer_destination = x.get_destination();
			destination_decoded = true;
			if (!(op1_decoded || (rlf.is_reg_locked(x.get_operand_1()) != -1))&&!contains(altered_destinations, x.get_operand_1()))
			{
				buffer_op1 = registers[x.get_operand_1()].get_register_value();
				op1_decoded = true;
			}
			buffer_op2 = x.get_operand_2();
			op2_decoded = true;
			break;
		case instructions::STOR:
		case instructions::BRR:
			if (!(destination_decoded || (rlf.is_reg_locked(x.get_destination()) != -1)) && !contains(altered_destinations, x.get_destination()))
			{
				buffer_destination = registers[x.get_destination()].get_register_value();
				destination_decoded = true;
			}
			if (!(op1_decoded || (rlf.is_reg_locked(x.get_operand_1()) != -1)) && !contains(altered_destinations, x.get_operand_1()))
			{
				buffer_op1 = registers[x.get_operand_1()].get_register_value();
				op1_decoded = true;
			}
			if (!(op2_decoded || (rlf.is_reg_locked(x.get_operand_2()) != -1)) && !contains(altered_destinations, x.get_operand_2()))
			{

				buffer_op2 = registers[x.get_operand_2()].get_register_value();
				op2_decoded = true;
			}
			break;
		case instructions::STO:
		case instructions::BRI:
			if (!(destination_decoded || (rlf.is_reg_locked(x.get_destination()) != -1)) && !contains(altered_destinations, x.get_destination()))
			{
				buffer_destination = registers[x.get_destination()].get_register_value();
				destination_decoded = true;
			}
			if (!(op1_decoded || (rlf.is_reg_locked(x.get_operand_1()) != -1)) && !contains(altered_destinations, x.get_operand_1()))
			{
				buffer_op1 = registers[x.get_operand_1()].get_register_value();
				op1_decoded = true;
			}
			buffer_op2 = x.get_operand_2();
			op2_decoded = true;
			break;
		case instructions::LDC:
			buffer_destination = x.get_destination();
			buffer_op1 = x.get_operand_1();
			destination_decoded = true;
			op1_decoded = true;
			op2_decoded = true;
			break;
		case instructions::JMPR:
			buffer_destination = program_counter;
			if (!(op1_decoded || (rlf.is_reg_locked(x.get_operand_1()) != -1)) && !contains(altered_destinations, x.get_operand_1()))
			{
				buffer_op1 = registers[x.get_destination()].get_register_value();
				op1_decoded = true;
			}
			buffer_op2 = x.get_operand_2();
			destination_decoded = true;

			op2_decoded = true;
			break;
		case instructions::JMP:
			buffer_destination = program_counter;
			buffer_op1 = x.get_operand_1();
			destination_decoded = true;
			op1_decoded = true;
			op2_decoded = true;
			break;
		default:
			break;


		}
		op1_decoded_buffer[i] = op1_decoded;
		op2_decoded_buffer[i] = op2_decoded;
		destination_decoded_buffer[i] = destination_decoded;
		op1_buffer[i]= buffer_op1;
		op2_buffer[i]= buffer_op2;
		destination_buffer[i] = buffer_destination;
		altered_destinations.push_back(destination_buffer[i]);
	}
	return true;
}
void decode_unit::flush_pipeline()
{
	is_ready_flag = true;
	op1_decoded = false;
	op2_decoded = false;
	destination_decoded = false;

	op1_decoded_buffer.clear();
	op2_decoded_buffer.clear();
	destination_decoded_buffer.clear();
	op1_buffer.clear();
	op2_buffer.clear();
	destination_buffer.clear();

	instruction_buffer_v.flush();
	pc_buffer_v.flush();

}
void decode_unit::replace_register_val(int64_t reg, int64_t val, register_lock_file& rlf)
{

	for (int64_t i = 0; i < instruction_buffer_v.avail_length(); i++)
	{


		instruction x{ instruction_buffer_v.get_item_at_index(i) };

		buffer_opcode = x.get_opcode();
		auto op1_decoded = op1_decoded_buffer.at(i);
		auto op2_decoded = op2_decoded_buffer.at(i);
		auto destination_decoded = destination_decoded_buffer.at(i);
		auto buffer_op1 = op1_buffer.at(i);
		auto buffer_op2 = op2_buffer.at(i);
		auto buffer_destination = destination_buffer.at(i);
		bool successful = false;
		switch (x.get_opcode())
		{
		case instructions::NOP://Ignore dest, op1 and op2.
		case instructions::LDC:
			break;
		case instructions::ADDR://all ops set as needed
		case instructions::MULR://all ops set as needed
		case instructions::CMPR://all ops set as needed
		case instructions::LDRR://all ops set as needed
			if ((!op1_decoded && reg == x.get_operand_1()))
			{
				buffer_op1 = val;
				op1_decoded = true;
				successful = true;
			}
			if ((!op2_decoded && reg == x.get_operand_2()))
			{
				buffer_op2 = val;
				op2_decoded = true;
				successful = true;
			}

			break;
		case instructions::ADDI:
		case instructions::MULI:
		case instructions::CMPI:
		case instructions::LDR:

			if ((!op1_decoded && reg == x.get_operand_1()))
			{
				buffer_op1 = val;
				op1_decoded = true;
				successful = true;
			}
			break;
		case instructions::STOR:
		case instructions::BRR:
			if ((!destination_decoded && reg == x.get_destination()))
			{
				buffer_destination = val;
				destination_decoded = true;
				successful = true;
			}
			if ((!op1_decoded && reg == x.get_operand_1()))
			{
				buffer_op1 = val;
				op1_decoded = true;
				successful = true;
			}
			if ((!op2_decoded && reg == x.get_operand_2()))
			{
				buffer_op2 = val;
				op2_decoded = true;
				successful = true;
			}
			break;
		case instructions::STO:
		case instructions::BRI:
			if ((!destination_decoded && reg == x.get_destination()))
			{
				buffer_destination = val;
				destination_decoded = true;
				successful = true;
			}

			if ((!op1_decoded && reg == x.get_operand_1()))
			{
				buffer_op1 = val;
				op1_decoded = true;
				successful = true;
			}
			break;

		case instructions::JMPR:
			//buffer_destination = program_counter;
			if ((!op1_decoded && reg == x.get_opcode()))
			{
				buffer_op1 = val;
				op1_decoded = true;
				successful = true;
			}
			break;

		default:
			break;


		}
		if (successful)
		{
			op1_decoded_buffer[i] = op1_decoded;
			op2_decoded_buffer[i] = op2_decoded;
			destination_decoded_buffer[i] = destination_decoded;
			op1_buffer[i] = buffer_op1;
			op2_buffer[i] = buffer_op2;
			destination_buffer[i] = buffer_destination;

		}
		if (buffer_destination == reg)
		{
			return;
		}
	}
}
void decode_unit::clock_tick()
{
	if (time_delay > 0) { --time_delay; }
	buffer_instruction.clock_tick();
	buffer_pc.clock_tick();

	instruction_buffer_v.clock_tick();
	pc_buffer_v.clock_tick();
	branch_issued = false;
}

void decode_unit::forward(execution_unit& exec, register_lock_file& rlf)
{
	bool branch_lock_off = false;
	for (int64_t i = 0; i < instruction_buffer_v.avail_length(); i++)
	{
		instruction x{ instruction_buffer_v.get_item_at_index(i) };
		buffer_opcode = x.get_opcode();
		if ((buffer_opcode == BRI || buffer_opcode == BRR || buffer_opcode == JMP || buffer_opcode == JMPR))
		{
			if ((((i == 0) && !op_completed(0)) || ((i != 0))))
			{
				exec.receive_issue(NOP, 0, 0, 0, 0);
				return;
			}
			if ((rlf.is_reg_locked(program_counter) != -1) || branch_issued)
			{
				continue;
			}
		}
		if (op_completed(i))
		{

			int64_t buffer_op1 = op1_buffer.at(i);
			int64_t buffer_op2 = op2_buffer.at(i);
			int64_t buffer_destination = destination_buffer.at(i);
			int64_t buffer_pc = pc_buffer_v.get_item_at_index(i);
			if (buffer_opcode == STO || buffer_opcode == STOR)
			{
				if ((rlf.is_reg_locked(program_counter) != -1) || branch_issued)
				{
					continue;
				}
			}
			exec.receive_issue(
				static_cast<instructions>(buffer_opcode),
				buffer_pc,
				buffer_destination,
				buffer_op1,
				buffer_op2);
			is_ready_flag = true;
			op1_decoded = false;
			op2_decoded = false;
			destination_decoded = false;//flag decoded instruction for removal

			op1_decoded_buffer.erase(op1_decoded_buffer.begin() + i);
			op2_decoded_buffer.erase(op2_decoded_buffer.begin() + i);
			destination_decoded_buffer.erase(destination_decoded_buffer.begin() + i);
			op1_buffer.erase(op1_buffer.begin() + i);
			op2_buffer.erase(op2_buffer.begin() + i);
			destination_buffer.erase(destination_buffer.begin() + i);

			instruction_buffer_v.remove_item_at_index(i);
			pc_buffer_v.remove_item_at_index(i);
			switch (x.get_opcode())
			{
			case instructions::NOP://Ignore dest, op1 and op2.
			case instructions::STOR:
			case instructions::STO:
				break;
			case instructions::BRR:
			case instructions::BRI:
			case instructions::JMPR:
			case instructions::JMP:
				branch_issued = true;
				rlf.lock_reg(program_counter, buffer_pc);
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

				rlf.lock_reg(x.get_destination(),buffer_pc);

				break;
			default:
				break;


			}
			return;
		}
	}
	exec.receive_issue(NOP, 0, 0, 0, 0);



}
