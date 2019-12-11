
#include "branch_predictor.h"

Branch_Predictor::Branch_Predictor(branch_predictor_types type, int64_t index_bits)
{
	type_of_predictor = type;
	int64_t history_entries = 2;
	--index_bits;
	for (auto i = 0; i < index_bits; i++)
	{
		history_entries = history_entries *history_entries;
	}
	for (auto i = 0; i < history_entries; i++)
	{
		history_table.push_back(History_entry{ -1,-1,0,0,std::vector<int64_t>{3,3,3,3} });
	}

}

void Branch_Predictor::receive_information(int64_t branch_pc, int64_t result, int64_t prediction)
{
	int64_t index = branch_pc & (history_table.size() - 1);
	auto& entry = history_table[index];
	int mod = 0;
	switch (type_of_predictor)
	{
	case Branch_Predictor::static_loopback:
		entry.Destination = result;
		entry.PC = branch_pc;
		break;
	case Branch_Predictor::saturating_counter:
		mod = (result == prediction) ? 1 : -1;
		if (entry.PC == branch_pc)
		{
			if (entry.saturating_counters[0] < 3) { entry.saturating_counters[0] -= mod; }
			else { entry.saturating_counters[0] += mod; }

			if (entry.saturating_counters[0] < 1) { entry.saturating_counters[0] = 1; }
			if (4 < entry.saturating_counters[0]) { entry.saturating_counters[0] = 4; }
		}
		else
		{
			entry.Destination = result;
			entry.PC = branch_pc;
			entry.saturating_counters[0] = (mod == 1) ? 2 : 3;

		}
		break;
	case Branch_Predictor::bit_history_2:
		mod = (result == prediction) ? 1 : -1;


		if (entry.PC == branch_pc)
		{
			bool branch_taken = (result == entry.Destination);
			entry.history = ((entry.history << 1) & 3);
			entry.history = (branch_taken) ? entry.history + 1 : entry.history;
			if (mod == -1)
			{
				entry.history_prediction = entry.history;
			}
			auto& saturating_counter = entry.saturating_counters[entry.history];
			bool predicted_result = (saturating_counter<3) ? -1 : 1;
			auto bias = (predicted_result == branch_taken) ? 1 : -1;
			if (saturating_counter < 3) { saturating_counter -= bias; }
			else { saturating_counter += bias; }


			if (saturating_counter < 1) { saturating_counter = 1; }
			if (4 < saturating_counter) { saturating_counter = 4; }
		}
		else
		{
			entry.Destination = result;
			entry.PC = branch_pc;
			for (auto& counter : entry.saturating_counters)
			{
				counter = (mod == 1) ? 2 : 3;
			}
		}
		break;
	default:
		break;
	}
}

int64_t Branch_Predictor::determine_next_pc(int64_t current_pc)
{
	int64_t index = current_pc & (history_table.size() - 1);
	auto& value = history_table[index];
	if (value.PC == current_pc)
	{
		switch (type_of_predictor)
		{
		case Branch_Predictor::static_loopback:
			if (value.Destination < current_pc) { return value.Destination; }
			return current_pc + 1;
			break;
		case Branch_Predictor::saturating_counter:
			if (value.saturating_counters[0] < 3) { return current_pc + 1;  }
			return  value.Destination;
			break;
		case Branch_Predictor::bit_history_2:
			value.history_prediction = ((value.history_prediction << 1) & 3);
			if (value.saturating_counters[value.history_prediction] < 3)
			{
				return current_pc + 1;
			}
			value.history_prediction += 1;
			return  value.Destination;
			break;
		default:
			break;
		}
	}
	else
	{
		return current_pc + 1;
	}
	return int64_t();
}
