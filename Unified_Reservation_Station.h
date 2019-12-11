#pragma once
#include "Instructions.h"
#include "ALU.h"
#include "AGU.h"
#include "Branching_Unit.h"

#ifndef URS_Included
#define URS_Included

class AGU;
class Unified_Reservation_Station
{
public:
	struct RS_entry {
		int64_t ROB_index;
		int64_t opcode;
		int64_t dest;
		int64_t op1;
		int64_t op2;
		bool dest_ready;
		bool op1_ready;
		bool op2_ready;

		int64_t pc;
		int64_t predicted_pc;
	};

	Unified_Reservation_Station(int64_t size);
	bool is_ready();
	void common_data_bus(int64_t ROB, int64_t value);
	void flush(int64_t index_start, int64_t index_end, int64_t size);
	int64_t receive_issue(RS_entry issue);
	int64_t get_available_space();
	void issue_alus(std::vector<ALU>& alus);
	void issue_agu(AGU& agu);
	void issue_bu(Branching_Unit& BU);
	void clock_tick();


private:
	std::vector<RS_entry> RS_Array;
	std::vector<RS_entry> RS_Next_Array;

	bool flushing = false;
	int64_t RS_size = 0;
};


#endif // !AGU_Included
