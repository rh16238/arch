
#include "Instruction_Issue_Unit.h"
#include "Instructions.h"

Instruction_Issue_Unit::Instruction_Issue_Unit(int64_t size)
{
	IIU_Array = std::vector<IIU_entry>{};
	IIU_Next_Array = std::vector<IIU_entry>{};
	this->IIU_size = size;
}

bool Instruction_Issue_Unit::is_ready()
{
	return (get_available_space() != 0) && !flushing;
}

void Instruction_Issue_Unit::flush()
{
	flushing = true;
	this->IIU_Next_Array.clear();
}

int64_t Instruction_Issue_Unit::receive_issue(int64_t i,int64_t pc, int64_t predicted_pc)
{
	if (!flushing)
	{
		instruction new_i{ i };
		IIU_Next_Array.push_back(IIU_entry{ new_i.get_opcode(),new_i.get_destination(),new_i.get_operand_1(),new_i.get_operand_2(),pc, predicted_pc });
	}
	return i;
}

int64_t Instruction_Issue_Unit::get_available_space()
{
	return IIU_size - IIU_Next_Array.size();
}


void Instruction_Issue_Unit::issue_Instructions(Unified_Reservation_Station& URS, Register_Alias_Table& RAT, Reorder_Buffer& ROB, std::vector<physical_register>& registers,Memory_Order_Buffer& MO, int64_t instructions_to_issue)//Take RAT
{

	std::vector<temp_RAT> RAT_results{};
	bool updated = false;
	for (size_t i = 0; i < 4; i++)//Number of instructions to issue.
	{
		if (!(ROB.is_ready())) { return; }
		if (i == IIU_Array.size()) { return; }
		auto& instruct = IIU_Array[i];
		Unified_Reservation_Station::RS_entry new_entry;

		new_entry.opcode = instruct.opcode;
		new_entry.ROB_index = ROB.reserve_ROB_index(instruct.dest, instruct.opcode);//Need to manipulate program counter RAT stuff on branches/stores
		new_entry.pc = instruct.PC;
		new_entry.predicted_pc = instruct.predicted_PC;
		register_lookup_result lookup;
		switch (instruct.opcode)
		{
		case instructions::NOP://Ignore dest, op1 and op2.
			new_entry.dest = 0;
			new_entry.op1 = 0;
			new_entry.op2 = 0;
			new_entry.dest_ready = true;
			new_entry.op1_ready = true;
			new_entry.op2_ready = true;
			break;
		case instructions::ADDR://all ops set as needed
		case instructions::MULR://all ops set as needed
		case instructions::CMPR://all ops set as needed
		case instructions::LDRR://all ops set as needed
			lookup = lookup_register(instruct.op1, RAT, ROB, registers, RAT_results);
			new_entry.op1 = lookup.result; new_entry.op1_ready = lookup.literal;

			lookup = lookup_register(instruct.op2, RAT, ROB, registers, RAT_results);
			new_entry.op2 = lookup.result; new_entry.op2_ready = lookup.literal;

			new_entry.dest = instruct.dest; new_entry.dest_ready = true;
			RAT.set_register(instruct.dest, new_entry.ROB_index);

			updated = false;
			for (auto& x : RAT_results)
			{
				if (x.reg == instruct.dest) { updated = true; x.Value = new_entry.ROB_index; }
			}
			if (!updated) { RAT_results.push_back(temp_RAT{ instruct.dest,new_entry.ROB_index }); }
			break;
		case instructions::ADDI:
		case instructions::MULI:
		case instructions::CMPI:
		case instructions::LDR:
			lookup = lookup_register(instruct.op1, RAT, ROB, registers, RAT_results);
			new_entry.op1 = lookup.result; new_entry.op1_ready = lookup.literal;

			new_entry.op2 = instruct.op2; new_entry.op2_ready = true;

			new_entry.dest = instruct.dest; new_entry.dest_ready = true;
			RAT.set_register(instruct.dest, new_entry.ROB_index);

			updated = false;
			for (auto& x : RAT_results)
			{
				if (x.reg == instruct.dest) { updated = true; x.Value = new_entry.ROB_index; }
			}
			if (!updated) { RAT_results.push_back(temp_RAT{ instruct.dest,new_entry.ROB_index }); }
			break;
		case instructions::STOR:
			lookup = lookup_register(instruct.op1, RAT, ROB, registers, RAT_results);
			new_entry.op1 = lookup.result; new_entry.op1_ready = lookup.literal;

			lookup = lookup_register(instruct.op2, RAT, ROB, registers, RAT_results);
			new_entry.op2 = lookup.result; new_entry.op2_ready = lookup.literal;

			lookup = lookup_register(instruct.dest, RAT, ROB, registers, RAT_results);
			new_entry.dest = lookup.result; new_entry.dest_ready = lookup.literal;//Need to manipulate program counter RAT stuff

			//RAT.set_register(instruct.dest, new_entry.ROB_index);//wrong
			//RAT_results.push_back(temp_RAT{ false,new_entry.ROB_index });
			break;
		case instructions::BRR:
			lookup = lookup_register(instruct.op1, RAT, ROB, registers, RAT_results);
			new_entry.op1 = lookup.result; new_entry.op1_ready = lookup.literal;

			lookup = lookup_register(instruct.op2, RAT, ROB, registers, RAT_results);
			new_entry.op2 = lookup.result; new_entry.op2_ready = lookup.literal;

			lookup = lookup_register(instruct.dest, RAT, ROB, registers, RAT_results);
			new_entry.dest = lookup.result; new_entry.dest_ready = lookup.literal;//Need to manipulate program counter RAT stuff

			//RAT.set_register(instruct.dest, new_entry.ROB_index);//wrong
			//RAT_results.push_back(temp_RAT{ false,new_entry.ROB_index });
			break;
		case instructions::STO:
			lookup = lookup_register(instruct.op1, RAT, ROB, registers, RAT_results);
			new_entry.op1 = lookup.result; new_entry.op1_ready = lookup.literal;

			new_entry.op2 = instruct.op2; new_entry.op2_ready = true;

			lookup = lookup_register(instruct.dest, RAT, ROB, registers, RAT_results);
			new_entry.dest = lookup.result; new_entry.dest_ready = lookup.literal;//Need to manipulate program counter RAT stuff

			//RAT.set_register(instruct.dest, new_entry.ROB_index);//wrong

			//RAT_results.push_back(temp_RAT{ false,new_entry.ROB_index });
			break;
		case instructions::BRI://Need to manipulate program counter RAT stuff
			lookup = lookup_register(instruct.op1, RAT, ROB, registers, RAT_results);
			new_entry.op1 = lookup.result; new_entry.op1_ready = lookup.literal;

			new_entry.op2 = instruct.op2; new_entry.op2_ready = true;

			lookup = lookup_register(instruct.dest, RAT, ROB, registers, RAT_results);
			new_entry.dest = lookup.result; new_entry.dest_ready = lookup.literal;//Need to manipulate program counter RAT stuff

			//RAT.set_register(instruct.dest, new_entry.ROB_index);//wrong
			//RAT_results.push_back(temp_RAT{ false,new_entry.ROB_index });

			break;
		case instructions::LDC:
			new_entry.op1 = instruct.op1; new_entry.op1_ready = true;

			new_entry.op2 = instruct.op2; new_entry.op2_ready = true;

			new_entry.dest=instruct.dest; new_entry.dest_ready = true;
			RAT.set_register(instruct.dest, new_entry.ROB_index);

			updated = false;
			for (auto& x : RAT_results)
			{
				if (x.reg == instruct.dest) { updated = true; x.Value = new_entry.ROB_index; }
			}
			if (!updated) { RAT_results.push_back(temp_RAT{ instruct.dest,new_entry.ROB_index }); }
			break;
		case instructions::JMPR:
			lookup = lookup_register(instruct.dest, RAT, ROB, registers, RAT_results);
			new_entry.op1 = lookup.result; new_entry.op1_ready = lookup.literal;

			new_entry.op2 = instruct.op1; new_entry.op2_ready = true;//This is corred, DEST + OP1

			new_entry.dest = program_counter; new_entry.dest_ready = true;
			//RAT.set_register(instruct.dest, new_entry.ROB_index);
			//RAT_results.push_back(temp_RAT{ instruct.dest,new_entry.ROB_index });
			break;
		case instructions::JMP:
			new_entry.op1 = instruct.op1; new_entry.op1_ready =true;

			new_entry.op2 = 0; new_entry.op2_ready = true;//This is correct, DEST, no op2

			new_entry.dest = program_counter; new_entry.dest_ready = true;

			break;
		default:
			break;
		}


		switch (instruct.opcode)
		{
		case STO:
		case STOR:
		case LDR:
		case LDRR:
			MO.add_op(new_entry.ROB_index, new_entry.opcode);
			break;
		default:
			break;
		}
		IIU_Next_Array.erase(IIU_Next_Array.begin());
		URS.receive_issue(new_entry);
	}
}

void Instruction_Issue_Unit::clock_tick()
{
	IIU_Array = IIU_Next_Array;
	flushing = false;
}

Instruction_Issue_Unit::register_lookup_result Instruction_Issue_Unit::lookup_register(int64_t reg, Register_Alias_Table & RAT, Reorder_Buffer & ROB, std::vector<physical_register>& registers,std::vector<temp_RAT>& RAT_results)
{
	int64_t RAT_lookup_result ;
	for (auto& x : RAT_results)
	{
		if (x.reg == reg)
		{
			RAT_lookup_result = x.Value;
			return register_lookup_result{ false,x.Value };//if its in this buffer, it cant have a literal
			break;

		}
	}
	RAT_lookup_result = RAT.lookup_register(reg); //Lookup RAT
	bool literal_available = (RAT_lookup_result == -1) ? true : false; //Has it been claimed on the NAT?
	if (literal_available)
	{
		return register_lookup_result{ true,registers[reg].get_register_value() };//If no, got get the value
	}

	literal_available = ROB.get_ROB_value_written(RAT_lookup_result);//If yes, does the ROB have the literal?
	if (literal_available) {return register_lookup_result{ true,ROB.get_ROB_value(RAT_lookup_result) };}//Execution was finished but has yet to be written
	else  return register_lookup_result{ false,RAT_lookup_result };//Still in execution

}
