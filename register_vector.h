#pragma once

#include "Instructions.h"
class register_vector
{
public:
	register_vector(int64_t max_size, std::vector<int64_t> default_vals);
	int64_t avail_length();
	int64_t next_length();
	int64_t get_max_size();
	int64_t get_item_at_index(int64_t index);
	int64_t remove_item_at_index(int64_t index);
	void    set_item_at_index(int64_t index, int64_t value);
	void    add_item(int64_t value);
	void    clock_tick();
	void    flush();
private:
	int64_t max_size = 1;
	std::vector<int64_t> temp_vals;
	std::vector<int64_t> vals;
};
