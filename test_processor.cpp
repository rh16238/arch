
#include "test_processor.h"
#include "processor_basic.h"
#include "program_store.h"
#include "processor_pipelined.h"
#include <algorithm>    // std::sort

bool test_ld()
{
	std::vector<instruction> program{
		instruction{ instructions::LDC,r0,256lu,0lu},
		instruction{ instructions::LDC,register_names::r1,512lu,0lu },
		instruction{ instructions::LDC,register_names::r1,200lu,0lu },
		instruction{ instructions::LDC,register_names::r2,3lu,0lu },
		instruction{ instructions::LDR,register_names::r3,r2,2 }
	};
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	memory.push_back(1996);
	processor_pipelined p{ memory };
	for (size_t i = 0; i < 250; i++)//set super high to allow me to increase memory load times
	{
		p.clock_tick();
	}
	auto ldc_r0 = p.return_register_value(register_names::r0);
	auto ldc_r1 = p.return_register_value(register_names::r1);
	auto ldr_r3 = p.return_register_value(register_names::r3);
	if (ldc_r0 != 256lu || ldc_r1 != 200||ldr_r3!=1996)
	{
		std::cout << "test ld failed, load did not complete as expected\n";
		return false;
	}

	std::cout << "test ld passed\n";
	return true;
}
bool test_addr()
{
	std::vector<instruction> program{
		instruction{ instructions::LDC,r0,256lu,0lu},
		instruction{ instructions::LDC,register_names::r1,512lu,0lu },
		instruction{ instructions::ADDR,register_names::r2,r1,r0 } };
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	processor_pipelined p{ memory };
	for (size_t i = 0; i < 250; i++)//set super high to allow me to increase memory load times
	{
		p.clock_tick();
	}
	auto addr_r2 = p.return_register_value(register_names::r2);
	if (addr_r2 != 768lu )
	{
		std::cout << "test addr failed \n";
		return false;
	}

	std::cout << "test addr passed\n";
	return true;
}
bool test_addi()
{
	std::vector<instruction> program{ instruction{ instructions::LDC,r0,256lu,0lu},instruction{ instructions::ADDI,r0,r0,256lu }};
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	processor_pipelined p{ memory };
	for (size_t i = 0; i < 100; i++)//set super high to allow me to increase memory load times
	{
		p.clock_tick();
	}
	auto addr_r0 = p.return_register_value(register_names::r0);
	if (addr_r0 != 512lu)
	{
		std::cout << "test addi failed \n";
		return false;
	}

	std::cout << "test addi passed\n";
	return true;
}
bool test_mul()
{
	std::vector<instruction> program{ instruction{ instructions::LDC,r0,256lu,0lu},instruction{ instructions::MULI,r1,r0,2lu}, instruction{instructions::LDC,r3,4lu,0lu },instruction{ instructions::MULR,r4,r3,r0} };
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	processor_pipelined p{ memory };
	for (size_t i = 0; i < 100; i++)//set super high to allow me to increase memory load times
	{
		p.clock_tick();
	}
	auto muli_r1 = p.return_register_value(register_names::r1);
	auto mulr_r4 = p.return_register_value(register_names::r4);
	if (muli_r1 != 512lu || mulr_r4 != 1024)
	{
		std::cout << "test mul failed \n";
		return false;
	}

	std::cout << "test mul passed\n";
	return true;
}
bool test_cmp()
{
	std::vector<instruction> program{
		instruction{ instructions::LDC,r0,256lu,0lu},
		instruction{ instructions::LDC,r0,256lu,0lu},
		instruction{ instructions::LDC,r1,4lu,0lu },
		instruction{ instructions::CMPR,r2,r0,r1},
		instruction{ instructions::CMPR,r3,r1,r0},
		instruction{ instructions::CMPR,r4,r1,r1},
		instruction{ instructions::CMPI,r5,r0,100},
		instruction{ instructions::CMPI,r6,r0,300},
		instruction{ instructions::CMPI,r7,r0,256}
	};
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	processor_pipelined p{ memory };
	for (size_t i = 0; i < 1000; i++)//set super high to allow me to increase memory load times
	{
		p.clock_tick();
	}
	auto cmpr_r2 = p.return_register_value(register_names::r2);
	auto cmpr_r3 = p.return_register_value(register_names::r3);
	auto cmpr_r4 = p.return_register_value(register_names::r4);
	if (cmpr_r2 != 1 || cmpr_r3 != -1 || cmpr_r4 != 0)
	{
		std::cout << "test cmpr failed \n";
		return false;
	}
	auto cmpi_r5 = p.return_register_value(register_names::r5);
	auto cmpi_r6 = p.return_register_value(register_names::r6);
	auto cmpi_r7 = p.return_register_value(register_names::r7);
	if (cmpi_r5 != 1 || cmpi_r6 != -1 || cmpi_r7 != 0)
	{
		std::cout << "test cmpr failed\n";
		return false;
	}
	std::cout << "test cmp passed\n";
	return true;
}

bool test_branch()
{
	std::vector<instruction> program{
		instruction{ instructions::LDC,r2,2,0},
		instruction{ instructions::LDC,r10,2,0},
		instruction{ instructions::ADDI,r1,r1,1 },
		instruction{ instructions::MULR,r2,r2,r2},
		instruction{ instructions::CMPI,r9,r1,3},
		instruction{ instructions::BRI,r10,r9,-1},
	};
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	processor_pipelined p{ memory };
	for (size_t i = 0; i < 1000; i++)//set super high to allow me to increase memory load times
	{
		p.clock_tick();
	}
	auto bri_r1 = p.return_register_value(register_names::r1);
	auto bri_r2 = p.return_register_value(register_names::r2);
	if (bri_r1 != 3 || bri_r2 != 256 )
	{
		std::cout << "test branch failed, branch did not complete as expected\n";
		return false;
	}

	std::cout << "test branch passed\n";
	return true;
}
bool test_sto()
{
	std::vector<instruction> program{
		instruction{ instructions::LDC,r0,100,0},
		instruction{ instructions::LDC,r1,5,0},
		instruction{ instructions::STO,r1,r0,5}
	};
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	processor_pipelined p{ memory };
	for (size_t i = 0; i < 100; i++)//set super high to allow me to increase memory load times
	{
		p.clock_tick();
	}
	auto memory_post = p.return_memory();
	if (memory_post[10] != 100)
	{
		std::cout << "test sto failed\n";
		return false;
    }

	std::cout << "test sto passed\n";
	return true;
}

bool test_vector_prog()
{
	std::vector<int64_t> a{ 0,1,2,3,4,5,6,7,8,-1,-2,-3,-4 };
	std::vector<int64_t> b{ 6,2,4,6,7,2,1,5,6,9,2,-5,1 };
	std::vector<int64_t> c;
	for (auto i = 0; i < a.size(); i++)
	{
		c.push_back(a[i] + b[i]);
	}
	auto memory = program_vector_addition(a, b);

	processor_pipelined p{ memory };
	while (p.return_register_value(program_counter) != naive_vector_addition_exit_pc())
	{
		p.clock_tick();
	}

	auto result = p.return_memory();
	auto c_mem_location = 20;
	for (auto i = 0; i < c.size(); i++)
	{
		if (result[20+i]!=c[i])
		{
			std::cout << "test vector program returned a different value than expected\n";
			return false;
		}
	}
	auto ops_performed = p.return_executed_instructions();
	auto cycles = p.return_clock_time();
	auto idle_time = p.return_idle_time();
	std::cout << "test vector program executed correctly, ops:" << ops_performed << " cycles: " << cycles << " Idle: " << idle_time << "\n";
	return true;
}



bool test_vector_bubble()
{
	std::vector<int64_t> a{ 6,2,4,6,7,2,1,5,6,9,2,-5,5,3,2,24,5,6,7,1,3 };//
	std::vector<int64_t> b{ a };
	std::sort(b.begin(), b.end());
	auto memory = program_bubble_sort(a);

	processor_pipelined p{ memory };
	auto pc = 0;
	while (p.return_register_value(program_counter) != naive_bubble_sort_exit_pc())
	{
		p.clock_tick();
	}

	auto result = p.return_memory();
	auto c_mem_location = 28;
	for (auto i = 0; i < b.size(); i++)
	{
		if (result[c_mem_location + i] != b[i])
		{
			std::cout << "test bubble sort program returned a different value than expected\n";
			return false;
		}
	}
	auto ops_performed = p.return_executed_instructions();
	auto cycles = p.return_clock_time();
	auto idle_time = p.return_idle_time();
	std::cout << "test bubble sort program executed correctly, ops:" << ops_performed << " cycles: " << cycles << " Idle: " << idle_time << "\n";
	return true;
}
bool test_naive_factorial()
{
	auto memory = program_naive_factorial(10);

	processor_pipelined p{ memory };
	while (p.return_register_value(program_counter) != naive_factorial_exit_pc())
	{
		p.clock_tick();
	}
	auto result = p.return_register_value(register_names::r1);


		if (result != 3628800)
		{
			std::cout << "test naive factorial program returned a different value than expected\n";
			return false;
		}
		auto ops_performed = p.return_executed_instructions();
		auto cycles = p.return_clock_time();
		auto idle_time = p.return_idle_time();
	std::cout << "test naive factorial program executed correctly, ops:" << ops_performed << " cycles: " << cycles << " Idle: " << idle_time << "\n";
	return true;
}


bool test_naive_quad_factorial()
{
	auto memory = program_quad_naive_factorial(10);

	processor_pipelined p{ memory };
	while (p.return_register_value(program_counter) != naive_quad_factorial_exit_pc())
	{
		p.clock_tick();
	}
	auto result1 = p.return_register_value(register_names::r1);
	auto result2 = p.return_register_value(register_names::r3);
	auto result3 = p.return_register_value(register_names::r5);
	auto result4 = p.return_register_value(register_names::r7);
	if (result1 != 3628800 || result2 != 3628800 || result3 != 3628800 || result4 != 3628800 )
	{
		std::cout << "test naive quad factorial program returned a different value than expected\n";
		return false;
	}
	auto ops_performed = p.return_executed_instructions();
	auto cycles = p.return_clock_time();
	auto idle_time = p.return_idle_time();
	std::cout << "test naive quad factorial program executed correctly, ops:"<< ops_performed <<" cycles: "<< cycles << " Idle: " <<idle_time <<"\n";
	return true;
}

bool test_throughput()
{
	std::vector<instruction> program{
		instruction{ instructions::LDC,r0,256lu,0lu}};
	for (size_t i = 0; i < 200; i++)
	{
		program.push_back(instruction{ instructions::NOP,register_names::r0,r0,r0 });
	}
	std::vector<int64_t> memory{};
	for (auto i : program)
	{
		memory.push_back(i.get_bit_representation());
	}
	processor_pipelined p{ memory };
	int i = 0;
	while (p.return_register_value(program_counter) < 200)
	{
		p.clock_tick();
		i++;
	}
	int64_t cycles = p.return_clock_time();
	int64_t ops = p.return_executed_instructions();
	int64_t itdle = p.return_idle_time();
	return true;
}

bool test_dot_product()
{

	std::vector<int64_t> a{ 0,1,2,3,4,5,6,7,8,-1,-2,-3,-4 };
	std::vector<int64_t> b{ 6,2,4,6,7,2,1,5,6,9,2,-5,1 };
	std::vector<int64_t> c;
	for (auto i = 0; i < a.size(); i++)
	{
		c.push_back(a[i] + b[i]);
	}
	auto memory = program_dot_product(a, b);

	processor_pipelined p{ memory };
	while (p.return_register_value(program_counter) != dot_product_exit_pc())
	{
		p.clock_tick();
	}

	auto result = p.return_register_value(r5);
	auto c_mem_location = 20;
	if (result != 153)
	{
		std::cout << "test dot product program returned a different value than expected\n";
		return false;
	}
	auto ops_performed = p.return_executed_instructions();
	auto cycles = p.return_clock_time();
	auto idle_time = p.return_idle_time();
	std::cout << "test dot product executed correctly, ops:" << ops_performed << " cycles: " << cycles << " Idle: " << idle_time << "\n";
	return true;
}
void test_unrolling()
{
	auto loops = 10;
	auto ops_per_loop = 100;
	auto rolled_memory = program_rolled_test(ops_per_loop, loops, true);
	processor_pipelined p_rolled{ rolled_memory };
	auto cycles = loops * ops_per_loop;
	for (size_t i = 0; i < cycles; i++)
	{
		p_rolled.clock_tick();
	}

	auto ops_performed_rolled = p_rolled.return_executed_instructions();
	auto cycles_rolled = p_rolled.return_clock_time();
	std::cout << "test rolled, ops:" << ops_performed_rolled << " cycles: " << cycles_rolled << "\n";


	auto unrolled_memory = program_rolled_test(ops_per_loop, loops, false);
	processor_pipelined p_unrolled{ unrolled_memory };
	for (size_t i = 0; i < cycles; i++)
	{
		p_unrolled.clock_tick();
	}
	auto ops_performed_unrolled = p_unrolled.return_executed_instructions();
	auto cycles_unrolled = p_unrolled.return_clock_time();
	std::cout << "test unrolled, ops:" << ops_performed_unrolled << " cycles: " << cycles_unrolled << "\n";
}


void test_unrolling_hazard()
{
	auto rolled_memory = program_hazard_unroll(true);
	processor_pipelined p_rolled{ rolled_memory };
	auto cycles = 1000;
	for (size_t i = 0; i < cycles; i++)
	{
		p_rolled.clock_tick();
	}
	auto result1 = p_rolled.return_register_value(r5);
	auto ops_performed_rolled = p_rolled.return_executed_instructions();
	auto cycles_rolled = p_rolled.return_clock_time();
	std::cout << "test hazard rolled, ops:" << ops_performed_rolled << " cycles: " << cycles_rolled << "\n";


	auto unrolled_memory = program_hazard_unroll(false);
	processor_pipelined p_unrolled{ unrolled_memory };
	for (size_t i = 0; i < cycles; i++)
	{
		p_unrolled.clock_tick();
	}
	auto result2 = p_unrolled.return_register_value(r5);
	auto ops_performed_unrolled = p_unrolled.return_executed_instructions();
	auto cycles_unrolled = p_unrolled.return_clock_time();
	std::cout << "test hazard unrolled, ops:" << ops_performed_unrolled << " cycles: " << cycles_unrolled << "\n";
}


bool test_processor()
{
	bool proc_tests_clean = true;
	test_throughput();
	proc_tests_clean &= test_ld();
	proc_tests_clean &= test_addr();
	proc_tests_clean &= test_addi();
	proc_tests_clean &= test_mul();
	proc_tests_clean &= test_cmp();
	proc_tests_clean &= test_branch();
	proc_tests_clean &= test_sto();

	proc_tests_clean &= test_dot_product();
	proc_tests_clean &= test_vector_prog();
	proc_tests_clean &= test_vector_bubble();
	proc_tests_clean &= test_naive_factorial();
	proc_tests_clean &= test_naive_quad_factorial();
	test_unrolling();
	test_unrolling_hazard();

	//test_vector_bubble_unrolled();
	return proc_tests_clean;
}
