
#include "register_vector.h"

register_vector::register_vector(int64_t max_size, std::vector<int64_t> default_vals)
{
	this->max_size = max_size;
	vals = default_vals;
	temp_vals = default_vals;
	vals.reserve(max_size);
	temp_vals.reserve(max_size);
}

int64_t register_vector::avail_length()
{
	return vals.size();
}

int64_t register_vector::next_length()
{
	return temp_vals.size();
}

int64_t register_vector::get_max_size()
{
	return max_size;
}

int64_t register_vector::get_item_at_index(int64_t index)
{
	if (static_cast<int64_t>(vals.size()) > index)
	{
		return vals.at(index);
	}
	throw;

}

int64_t register_vector::remove_item_at_index(int64_t index)
{
	if (static_cast<int64_t>(vals.size()) > index)
	{
		auto x =  vals.at(index);
		vals.erase(vals.begin() + index);
		temp_vals.erase(temp_vals.begin() + index);
		return x;
	}
	throw;
}

void register_vector::set_item_at_index(int64_t index, int64_t value)
{
	if (static_cast<int64_t>(vals.size()) > index)
	{
		temp_vals[index] = value;
		return;
	}
	throw;
}

void register_vector::add_item(int64_t value)
{
	if (static_cast<int64_t>(temp_vals.size()) < max_size)
	{
		temp_vals.push_back(value);
		return;
	}
	throw;
}

void register_vector::clock_tick()
{
	vals = temp_vals;
}

void register_vector::flush()
{
	temp_vals.clear();
	vals.clear();
}
