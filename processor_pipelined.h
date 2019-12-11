#pragma once
#include "math_utils.h"
#include "Register.h"
#include <vector>
#include "Instructions.h"
#include "fetch_unit.h"
#include "Reorder_Buffer.h"
#include "ALU.h"
#include "AGU.h"
#include "Unified_Reservation_Station.h"
#include "Register_Alias_Table.h"
#include "Instruction_Issue_Unit.h"
#include "Branching_Unit.h"
#include "branch_predictor.h"
#include "Memory_Order_Buffer.h"
class processor_pipelined
{
public:
	void clock_tick();
	processor_pipelined(const std::vector<int64_t> memory_stat);
	int64_t return_register_value(register_names reg);
	void print(std::ostream& os);
	const std::vector<int64_t>& return_memory();
	int return_idle_time();
	int return_executed_instructions();
	int return_clock_time();
private:
	int idle_time = 0;
	int executed_instructions = 0;
	int clock_ticks = 0;
	int speed_up_factor = 4;
	int speed_up_index = 0;

	std::vector<physical_register> registers{
	physical_register{ register_names::status,0lu },
	physical_register{ register_names::program_counter,0lu },
	physical_register{ register_names::memory_buffer,0lu } ,
	physical_register{ register_names::memory_address,0lu },
	physical_register{ register_names::instruction_register,0lu },
	physical_register{ register_names::r0,0lu },
	physical_register{ register_names::r1,0lu },
	physical_register{ register_names::r2,0lu },
	physical_register{ register_names::r3,0lu },
	physical_register{ register_names::r4,0lu },
	physical_register{ register_names::r5,0lu },
	physical_register{ register_names::r6,0lu },
	physical_register{ register_names::r7,0lu },
	physical_register{ register_names::r8,0lu },
	physical_register{ register_names::r9,0lu },
	physical_register{ register_names::r10,0lu},
	physical_register{ register_names::r11,0lu },
	physical_register{ register_names::r12,0lu },
	physical_register{ register_names::r13,0lu },
	physical_register{ register_names::r14,0lu },
	physical_register{ register_names::r15,0lu },
	physical_register{ register_names::accumulator,0lu },
	physical_register{ register_names::buffer_opcode,0lu },
	physical_register{ register_names::buffer_destination,0lu },
	physical_register{ register_names::buffer_op1,0lu },
	physical_register{ register_names::buffer_op2,0lu }, };

	fetch_unit fet_unit{};
	std::vector<int64_t> test_memory{ 1024,0 };

	Reorder_Buffer ROB{ 32 };
	std::vector<ALU> alu_units{ 4,ALU{} };
	AGU agu{};
	Branching_Unit BU{};
	Unified_Reservation_Station URS{ 32 };//16
	Register_Alias_Table RAT{ 64 };
	Instruction_Issue_Unit IIU{ 32 };//16
	Branch_Predictor BP{ Branch_Predictor::branch_predictor_types::saturating_counter,3 };
	Memory_Order_Buffer MO{ 32 };

};