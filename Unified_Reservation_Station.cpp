#include "Unified_Reservation_Station.h"

Unified_Reservation_Station::Unified_Reservation_Station(int64_t size)
{
	RS_Array = std::vector<RS_entry>{};
	RS_Next_Array = std::vector<RS_entry>{};
	this->RS_size = size;
}

bool Unified_Reservation_Station::is_ready()
{
	return (RS_Next_Array.size() != RS_size)&&!flushing;
}

void Unified_Reservation_Station::common_data_bus(int64_t ROB, int64_t value)
{
	for (auto& x : RS_Next_Array)
	{

		if ((!x.dest_ready) && (ROB == x.dest))
		{

			x.dest = value; x.dest_ready = true;
		}
		if ((!x.op1_ready) && (ROB == x.op1))
		{
			x.op1 = value; x.op1_ready = true;
		}
		if ((!x.op2_ready) && (ROB == x.op2))
		{
			x.op2 = value; x.op2_ready = true;
		}
	}
}

void Unified_Reservation_Station::flush(int64_t index_start, int64_t index_end, int64_t size)
{
	int offset = 0;
	for (size_t i = 0; i < RS_Array.size(); i++)
	{
		auto ROB_index = RS_Array[i].ROB_index;
		if (index_start <= index_end)
		{
			if (ROB_index <= index_end && index_start <= ROB_index)//ordinary check
			{
				auto next_index = i - offset;
				RS_Next_Array.erase(RS_Next_Array.begin() + next_index);
				offset++;
				flushing = true;
			}
		}
		else
		{
			if (ROB_index <= index_end || index_start <= ROB_index)//loops over size index
			{
				auto next_index = i - offset;
				RS_Next_Array.erase(RS_Next_Array.begin() + next_index);
				offset++;
				flushing = true;
			}
		}
	}

}

int64_t Unified_Reservation_Station::receive_issue(RS_entry issue)
{
	RS_Next_Array.push_back(issue);
	return int64_t();
}

int64_t Unified_Reservation_Station::get_available_space()
{
	return RS_size - RS_Next_Array.size();
}

void Unified_Reservation_Station::issue_alus( std::vector<ALU>& alus)
{
	int alu_index = 0;
	int offset = 0;
	for (size_t index = 0; index < RS_Array.size(); index++)
	{
		auto i = RS_Array[index];
		switch (i.opcode)//Weed out non alu instructions
		{
		case instructions::NOP:
		case instructions::ADDR:
		case instructions::ADDI:
		case instructions::MULR:
		case instructions::MULI:
		case instructions::CMPR:
		case instructions::CMPI:
		case instructions::LDC:
			break;
		default:
			continue;
			break;
		}
		if (i.op1_ready&&i.op2_ready&&i.dest_ready)//if alu instruction is ready
		{
			bool handled = false;
			while ((!handled) && alu_index != alus.size())
			{
				if (alus[alu_index].is_ready()) //if alu is ready
				{
					alus[alu_index].receive_issue(i.opcode, i.dest, i.op1, i.op2, i.ROB_index);//handle alu
					auto next_index = index - offset;
					RS_Next_Array.erase(RS_Next_Array.begin() + next_index);
					offset++;
					handled = true;

				}
				++alu_index;//check next alu
			}
		}
	}

}

void Unified_Reservation_Station::issue_agu(AGU& agu)
{
	//int alu_index = 0;
	//int offset = 0;
	for (size_t index = 0; index < RS_Next_Array.size(); index++)
	{
		auto i = RS_Next_Array[index];
		switch (i.opcode)//Weed out non alu instructions
		{
		case instructions::STO:
		case instructions::STOR:
		case instructions::LDR:
		case instructions::LDRR:
			break;
		default:
			continue;
			break;
		}
		if (i.op1_ready&&i.op2_ready&&i.dest_ready)//if alu instruction is ready
		{
			if (agu.is_ready())
			{
				agu.receive_issue(i.opcode, i.dest, i.op1, i.op2, i.ROB_index);
				//auto next_index = index - offset;
				RS_Next_Array.erase(RS_Next_Array.begin() + index);
				return;
			}
			//bool handled = false;
			//while ((!handled) && alu_index != alus.size())
			//{
			//	if (alus[alu_index].is_ready()) //if alu is ready
			//	{
			//		alus[alu_index].receive_issue(i.opcode, i.dest, i.op1, i.op2, i.ROB_index);//handle alu
			//		auto next_index = index - offset;
			//		RS_Next_Array.erase(RS_Next_Array.begin() + next_index);
			//		offset++;
			//		handled = true;

			//	}
			//	++alu_index;//check next alu
			//}
		}
	}
}

void Unified_Reservation_Station::issue_bu(Branching_Unit& BU)
{
	//int alu_index = 0;
	//int offset = 0;
	for (size_t index = 0; index < RS_Next_Array.size(); index++)
	{
		auto i = RS_Next_Array[index];
		switch (i.opcode)//Weed out non alu instructions
		{
		case instructions::BRI:
		case instructions::BRR:
		case instructions::JMP:
		case instructions::JMPR:
			break;
		default:
			continue;
			break;
		}
		if (i.op1_ready&&i.op2_ready&&i.dest_ready)//if alu instruction is ready
		{
			if (BU.is_ready())
			{
				BU.receive_issue(i.opcode, i.dest, i.op1, i.op2, i.ROB_index,i.pc,i.predicted_pc);
				RS_Next_Array.erase(RS_Next_Array.begin() + index);
				return;
			}
		}
	}
}
void Unified_Reservation_Station::clock_tick()
{
	RS_Array = RS_Next_Array;
	flushing = false;
}
