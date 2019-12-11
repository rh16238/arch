#pragma once

#include "Instructions.h"
#include "Unified_Reservation_Station.h"
class Register_Alias_Table
{
public:
	struct RAT_entry {
		int64_t ROB;
	};
	Register_Alias_Table(int64_t size);
	//void flush(int64_t index_start, int64_t index_end, int64_t size);
	void clock_tick();

	int64_t lookup_register(int64_t reg);
	int64_t set_register(int64_t reg, int64_t ROB);
	void free_register(int64_t reg, int64_t ROB);
	void free_references(int64_t rob_index);
private:
	std::vector<RAT_entry> RAT_Array;
	std::vector<RAT_entry> RAT_Next_Array;

	//bool flushing = false;
	int64_t RAT_size = 0;
};
