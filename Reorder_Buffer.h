#pragma once

#include "Instructions.h"
#include "Register_Alias_Table.h"
#include "Memory_Order_Buffer.h"

class Reorder_Buffer
{
public:
	struct ROB_Flush_info {
		int64_t Start_Index;
		int64_t Stop_Index;
		int64_t ROB_size;
	};

	Reorder_Buffer(int64_t size);
	bool is_ready();
	void common_data_bus(int64_t ROB, int64_t value);
	ROB_Flush_info flush_on(int64_t ROB, Register_Alias_Table& RAT);
	int64_t reserve_ROB_index(int64_t destination, int64_t opcode);
	void write_back(int entries_to_write, std::vector<physical_register>& registers, Register_Alias_Table& RAT,Memory_Order_Buffer& MO);
	void clock_tick();
	int64_t get_ROB_value(int64_t ROB_index);
	bool get_ROB_value_written(int64_t ROB);
	int64_t get_ops_performed();
	int64_t get_idle_ops_performed();
	void test_flush(Memory_Order_Buffer& MO);

private:
	struct ROB_entry {
		int64_t dest;
		int64_t opcode;
		int64_t value;
		bool value_set;
	};
	std::vector<ROB_entry> ROB;
	std::vector<ROB_entry> Next_ROB;
	int64_t add_entry(std::vector<ROB_entry>& ROB, int64_t& start_index, int64_t& next_index, int64_t dest, int64_t opcode);
	void delete_entry(std::vector<ROB_entry>& ROB, int64_t& start_index, int64_t& next_index);
	bool write_ROB(std::vector<physical_register>& registers, Register_Alias_Table& RAT, Memory_Order_Buffer& MO);
	int64_t ROB_start = 0;
	int64_t ROB_avail = 0;
	int64_t ROB_next_start = 0;
	int64_t ROB_next_avail = 0;
	int64_t ROB_size = 0;
	int64_t next_pc = 0;
	int64_t ops_performed=0;
	int64_t idle_time=0;
};
