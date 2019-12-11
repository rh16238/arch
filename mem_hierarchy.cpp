
#include "mem_hierarchy.h"

memory_hierarchy::memory_hierarchy(const std::vector<int64_t> memory_state, physical_register& mar_ref, physical_register& mbr_ref, physical_register& status_ref) : memory(memory_state), mar(mar_ref),mbr(mbr_ref), status_register(status_ref)
{
}

int64_t memory_hierarchy::access_location(const int64_t address)
{

	return memory.access_location(address);
}

void memory_hierarchy::set_location(const int64_t address, const int64_t value)
{

	memory.set_location(address,value);
}

void memory_hierarchy::clock_tick()
{
	if (wait_time != 0)
	{
		wait_time--;
		return;
	}
	auto status_val = status_register.get_register_value();
	if (status_val & memory_read)
	{
		wait_time = memory.time_cost;
		status_register.set_register_value(status_val ^ memory_op_fin ^memory_wait^memory_read);
		mbr.set_register_value(memory.access_location(mar.get_register_value()));
	}
	else if (status_val & memory_write)
	{
		memory.set_location(mar.get_register_value(), mbr.get_register_value());
		status_register.set_register_value(status_val ^ memory_op_fin ^memory_wait^memory_write);
	}
}

const std::vector<int64_t>& memory_hierarchy::return_memory()
{
	return memory.return_memory();
}
