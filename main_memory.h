#pragma once
#include "math_utils.h"

class main_memory
{
public:
	const static int time_cost = 20;
	main_memory(const std::vector<int64_t>& memory_bank);
	main_memory();
	int64_t access_location(int64_t memory_location);
	void set_location(int64_t memory_location, int64_t value);
	void clock_tick();
	const std::vector<int64_t>& return_memory();
private:
	const static int mem_size = 1024;
	std::vector<int64_t> memory_bank{ 1024,0 };

};