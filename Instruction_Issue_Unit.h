#pragma once

#include "Instructions.h"
#include "Unified_Reservation_Station.h"
#include "Register_Alias_Table.h"
#include "Reorder_Buffer.h"
#include "Memory_Order_Buffer.h"
class Instruction_Issue_Unit
{
public:
	struct IIU_entry {
		int64_t opcode;
		int64_t dest;
		int64_t op1;
		int64_t op2;
		int64_t PC;
		int64_t predicted_PC;
	};
	Instruction_Issue_Unit(int64_t size);
	bool is_ready();
	//void common_data_bus(int64_t ROB, int64_t value);
	void flush();
	int64_t receive_issue(int64_t i, int64_t pc, int64_t predicted_pc);
	int64_t get_available_space();
	void issue_Instructions(Unified_Reservation_Station& URS, Register_Alias_Table& RAT, Reorder_Buffer& ROB, std::vector<physical_register>& registers,Memory_Order_Buffer& MO, int64_t instructions_to_issue);
	void clock_tick();


private:
	std::vector<IIU_entry> IIU_Array;
	std::vector<IIU_entry> IIU_Next_Array;
	struct register_lookup_result {
		bool literal;
		int64_t result;
	};
	struct temp_RAT {
		int64_t reg;
		int64_t Value;
	};

	register_lookup_result lookup_register(int64_t reg, Register_Alias_Table & RAT, Reorder_Buffer & ROB, std::vector<physical_register>& registers, std::vector<temp_RAT>& RAT_results);
	bool flushing = false;
	int64_t IIU_size = 0;
};
