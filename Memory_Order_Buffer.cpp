
#include "Memory_Order_Buffer.h"

Memory_Order_Buffer::Memory_Order_Buffer(int64_t size)
{
	this ->MO_size = size;
}

bool Memory_Order_Buffer::is_ready()
{
	return Next_MO_Buffer.size() != MO_size;
}

void Memory_Order_Buffer::clock_tick()
{
	MO_buffer = Next_MO_Buffer;
}

void Memory_Order_Buffer::add_op(int64_t ROB, int64_t opcode)
{
	Next_MO_Buffer.push_back(MO_entry{ -1,false,opcode,-1,false,ROB,false });
}

void Memory_Order_Buffer::update_op(int64_t dest, int64_t value, int64_t opcode, int64_t ROB)
{
	for (auto& x : Next_MO_Buffer)
	{
		if (x.ROB == ROB)
		{

			x.addr = dest;
			x.addr_set = true;
			x.value = value;
			x.value_set = true;
			if (x.opcode == LDR || x.opcode == LDRR)
			{
			x.graduable = true;
			}
		}
	}
}

void Memory_Order_Buffer::allow_write_graduate(int64_t ROB)
{
	for (auto& x : Next_MO_Buffer)
	{
		if (x.ROB == ROB) { x.graduable = true; }
	}
}

Memory_Order_Buffer::MO_op_return Memory_Order_Buffer::perform_op(std::vector<int64_t>& memory)
{

	Memory_Order_Buffer::MO_op_return val { -1,-1 };
	if (MO_buffer.size() == 0) { return val; }
	bool read_start = (Next_MO_Buffer[0].opcode == LDR || Next_MO_Buffer[0].opcode == LDRR);
	for (size_t i = 0; i < Next_MO_Buffer.size(); i++)
	{
		auto& x = Next_MO_Buffer[i];
		if (x.graduable && x.addr_set && x.value_set)
		{
			val = graduate_op(x,memory);
			Next_MO_Buffer.erase(Next_MO_Buffer.begin() + i);
			return val;
		}
		else
		{
			return val;
		}
	}
	return val;
}

void Memory_Order_Buffer::flush(int64_t index_start, int64_t index_end, int64_t size)
{
	//Next_MO_Buffer.clear();
	int offset = 0;
	bool load_store_first = false;
	bool load_store_last = false;
	for (size_t i = 0; i < Next_MO_Buffer.size(); i++)
	{
		auto ROB_index = Next_MO_Buffer[i].ROB;
		if (index_start <= index_end)
		{
			if (ROB_index <= index_end && index_start <= ROB_index)//ordinary check
			{
				//auto next_index = i - offset;

				Next_MO_Buffer.erase(Next_MO_Buffer.begin() + i);
				--i;
				offset++;
			}
		}
		else
		{
			if (ROB_index <= index_end || index_start <= ROB_index)//loops over size index
			{
				//auto next_index = i - offset;
				Next_MO_Buffer.erase(Next_MO_Buffer.begin() + i);
				--i;
				offset++;
			}
		}
	}
}

void Memory_Order_Buffer::relink()
{
	for (auto&x : Next_MO_Buffer)
	{
		for (auto&x2 : MO_buffer)
		{
			if (x2.ROB == x.ROB)
			{
				x.addr = x2.addr;
				x.addr_set = x2.addr_set;
				x.graduable = x2.graduable;
				x.value = x2.value;
				x.value_set = x2.value_set;
			}
		}
	}
}

Memory_Order_Buffer::MO_op_return Memory_Order_Buffer::graduate_op(MO_entry op, std::vector<int64_t>& memory)
{
	Memory_Order_Buffer::MO_op_return val{ -1,op.ROB };
	switch (op.opcode)
	{
	case instructions::STO:
	case instructions::STOR:
		memory[op.addr] = op.value;
		val.Value = 0;
		break;
	case instructions::LDR:
	case instructions::LDRR:
		val.Value = memory[op.addr];
		break;
	}
	return val;
}
