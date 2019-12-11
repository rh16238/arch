#pragma once

#include "Instructions.h"
#include "Register_Alias_Table.h"
class Memory_Order_Buffer
{
public:
	struct MO_op_return {
		int64_t Value;
		int64_t ROB;
	};
	Memory_Order_Buffer(int64_t size);
	bool is_ready();
	void clock_tick();
	void add_op(int64_t ROB,int64_t opcode);
	void update_op(int64_t dest, int64_t value, int64_t opcode, int64_t ROB);
	void allow_write_graduate(int64_t ROB);
	MO_op_return perform_op(std::vector<int64_t>& memory);
	void flush(int64_t index_start, int64_t index_end, int64_t size);
	void relink();
private:
	struct MO_entry {
		int64_t addr;
		bool addr_set;
		int64_t opcode;
		int64_t value;
		bool value_set;
		int64_t ROB;
		bool graduable;
	};

	struct MO_single_associated_cache {
		int64_t full_addr;
		int64_t value;
		bool value_set;
		int64_t ROB;
		bool graduable;
	};
	std::vector<MO_entry> MO_buffer{};
	std::vector<MO_entry> Next_MO_Buffer{};
	MO_op_return graduate_op(MO_entry op, std::vector<int64_t>& memory);

	int64_t MO_size;
};
