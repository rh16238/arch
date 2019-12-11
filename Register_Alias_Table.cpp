
#include "Register_Alias_Table.h"

Register_Alias_Table::Register_Alias_Table(int64_t size)
{
	this->RAT_size = size;
	RAT_Next_Array = std::vector<RAT_entry>{ (long unsigned int)size,RAT_entry{(int64_t)-1} };
	RAT_Array = RAT_Next_Array;
}

void Register_Alias_Table::clock_tick()
{
	RAT_Array = RAT_Next_Array;

}

int64_t Register_Alias_Table::lookup_register(int64_t reg)
{
	return RAT_Array[reg].ROB;
}

int64_t Register_Alias_Table::set_register(int64_t reg,int64_t ROB)
{
	RAT_Next_Array[reg].ROB = ROB;
	return ROB;
}
void Register_Alias_Table::free_register(int64_t reg, int64_t ROB)
{

	if (RAT_Next_Array[reg].ROB == ROB) { RAT_Next_Array[reg].ROB = -1; }
}
void Register_Alias_Table::free_references(int64_t rob_index)
{
	for (auto& x : RAT_Next_Array)
	{
		if (x.ROB == rob_index) { x.ROB = -1; }
	}
}
