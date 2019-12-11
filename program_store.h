#pragma once
#include "Instructions.h"
#include "Registers.h"

enum program_list
{
	vector_addition,
	naive_factorial,
	quad_naive_factorial,
	bubble_sort,
};
std::vector<int64_t> program_dot_product(std::vector<int64_t> a, std::vector<int64_t> b);
int dot_product_exit_pc();
std::vector<int64_t> program_rolled_test(int64_t ops_per_loop, int64_t loops, bool return_rolled);
std::vector<int64_t> program_naive_factorial(int64_t factorial);
int naive_factorial_exit_pc();
std::vector<int64_t> program_vector_addition(std::vector<int64_t> a, std::vector<int64_t> b );
int naive_vector_addition_exit_pc();
std::vector<int64_t> program_bubble_sort(std::vector<int64_t> a);
int naive_bubble_sort_exit_pc();
std::vector<int64_t> program_quad_naive_factorial(int64_t factorial);
int naive_quad_factorial_exit_pc();
std::vector<int64_t> program_hazard_unroll(bool unroll);
