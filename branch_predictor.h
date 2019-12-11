#pragma once

#include "Instructions.h"
class Branch_Predictor
{
public:
	enum branch_predictor_types
	{
		static_loopback,
		saturating_counter,
		bit_history_2,
	};
	Branch_Predictor(branch_predictor_types type, int64_t index_bits);
	void receive_information(int64_t program_counter, int64_t result, int64_t prediction);
	int64_t determine_next_pc(int64_t current_pc);
	void clock_tick();

private:
	struct History_entry {
		int64_t PC;
		int64_t Destination;
		int64_t history;
		int64_t history_prediction;
		std::vector<int64_t> saturating_counters{ 3,3,3,3 };
	};
	std::vector<History_entry> history_table{};
	branch_predictor_types type_of_predictor = static_loopback;
	int64_t index_size;
};
