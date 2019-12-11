
#include "main_memory.h"

main_memory::main_memory(const std::vector<int64_t>& memory_bank_to_copy) : main_memory()
{
	if (memory_bank.size() < memory_bank_to_copy.size()) { memory_bank.resize(memory_bank_to_copy.size()); }
	std::copy(memory_bank_to_copy.begin(), memory_bank_to_copy.end(), memory_bank.begin());
}

main_memory::main_memory()
{
	memory_bank.resize(1024);
	std::fill(memory_bank.begin(), memory_bank.end(), 0);
}

int64_t main_memory::access_location(int64_t memory_location)
{
	if (memory_location > static_cast<int64_t>(memory_bank.size()) || memory_location < 0)
	{
		std::cout << "An error has occurred while accessing memory location " << memory_location;
	}
	return memory_bank[memory_location];
}

void main_memory::set_location(int64_t memory_location, int64_t value)
{
	if (memory_location > static_cast<int64_t>(memory_bank.size()) || memory_location < 0)
	{
		std::cout << "An error has occurred while setting memory location " << memory_location;
	}
	 memory_bank[memory_location] = value;
}

void main_memory::clock_tick()
{
}

const std::vector<int64_t>& main_memory::return_memory()
{
	return memory_bank;
}
