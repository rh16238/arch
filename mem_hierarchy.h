#pragma once
#include "math_utils.h"
#include "main_memory.h"
#include "Register.h"
class memory_hierarchy
{
public:
	memory_hierarchy(const std::vector<int64_t> memory_state, physical_register& mar_ref, physical_register& mbr_ref, physical_register& status_ref);
	int64_t access_location(const int64_t memory_location);
	void set_location(const int64_t memory_location, const int64_t value);
	void clock_tick();
	const std::vector<int64_t>& return_memory();
private:
	int wait_time = 0;
	main_memory memory;
	physical_register& mbr;
	physical_register& mar;
	physical_register& status_register;
};