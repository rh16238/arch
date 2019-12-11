
#include "test_mem.h"
#include "mem_hierarchy.h"
bool test_memory_initial()
{
	auto mar = physical_register{ register_names::memory_buffer,0lu };
	auto mdr = physical_register{ register_names::memory_address,0lu };
	auto status = physical_register{ register_names::status,0lu };
	memory_hierarchy test{ std::vector<int64_t>{100lu,225lu,20052lu,10000lu },mar,mdr,status };
	if (test.access_location(4lu) != 0lu || test.access_location(1023lu) != 0lu || test.access_location(2lu) != 20052lu)
	{
		std::cout << "Test memory initial failed, memory not as expected\n";
		return false;
	}
	std::cout << "Test memory initial passed\n";
	return true;
}
bool test_memory_store()
{
	auto mar = physical_register{ register_names::memory_buffer,0lu };
	auto mdr = physical_register{ register_names::memory_address,0lu };
	auto status = physical_register{ register_names::status,0lu };
	memory_hierarchy test{ std::vector<int64_t>{100lu,225lu,20052lu,10000lu },mar,mdr,status };
	if (test.access_location(4lu) != 0lu || test.access_location(1023lu) != 0lu || test.access_location(2lu) != 20052lu)
	{
		std::cout << "Test memory store failed, initial memory not as expected\n";
		return false;
	}
	test.set_location(2lu, 666lu);
	test.set_location(1023lu, 222lu);
	if (test.access_location(1023lu) != 222lu || test.access_location(2lu) != 666lu)
	{
		std::cout << "Test memory store failed, stored memory not as expected\n";
		return false;
	}
	std::cout << "Test memory store passed\n";
	return true;
}

bool test_memory()
{
	bool memory_clean = true;
	memory_clean &= test_memory_initial();
	memory_clean &= test_memory_store();
	return memory_clean;
}
