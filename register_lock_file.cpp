
#include "register_lock_file.h"

int64_t register_lock_file::is_reg_locked(int64_t reg)
{
	return registers[reg].get_register_value() ;
}

void register_lock_file::lock_reg(int64_t reg, int64_t pc)
{
	registers[reg].set_register_value(pc);
}

void register_lock_file::unlock_reg(int64_t reg)
{

	registers[reg].set_register_value(-1);
}

void register_lock_file::clock_tick()
{
	for (auto& x : registers)
	{
		x.clock_tick();
	}
}
