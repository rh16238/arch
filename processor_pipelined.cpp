
#include "processor_pipelined.h"
#include "Instructions.h"
void processor_pipelined::clock_tick()
{
	if (speed_up_index != speed_up_factor - 1)
	{
	//	++clock_ticks;
		ROB.write_back(1, registers, RAT,MO);
		++speed_up_index;
		return;
	}
	speed_up_index = 0;
	++clock_ticks;
	ROB.write_back(1, registers, RAT,MO);

	//ROB.write_back(4, registers,RAT);
	bool finished = BU.execute_instruction();
	bool flush_pipeline = false;
	if (finished)
	{
		ROB.common_data_bus(BU.get_ROB(), BU.get_result());
		flush_pipeline = BU.get_predictive_error();
		BP.receive_information(BU.get_pc(), BU.get_result(), BU.get_predicted_pc());
	}
	if (flush_pipeline)
	{


		fet_unit.flush_pipeline(BU.get_result());
		IIU.flush();
		auto result = ROB.flush_on(BU.get_ROB(),RAT);
		if (result.Start_Index != -1)
		{

			MO.flush(result.Start_Index, result.Stop_Index, result.ROB_size);
			URS.flush(result.Start_Index, result.Stop_Index, result.ROB_size);
			for (auto& x : alu_units) { x.flush(result.Start_Index, result.Stop_Index, result.ROB_size); }
			agu.flush(result.Start_Index, result.Stop_Index, result.ROB_size);
		}
	}
	else
	{


		for (auto& x : alu_units) { x.execute_instruction(); }
		auto agu_CDB_flagged = agu.execute_instruction(test_memory,MO);

		URS.issue_alus(alu_units);
		URS.issue_agu(agu);
		URS.issue_bu(BU);
		IIU.issue_Instructions(URS, RAT, ROB, registers, MO, 8);
		fet_unit.return_instruction(IIU, 8);
		fet_unit.fetch_instruction(test_memory, registers, BP,8);


		auto result = MO.perform_op(test_memory);
		if (result.ROB != -1)
		{
			ROB.common_data_bus(result.ROB, result.Value);
			URS.common_data_bus(result.ROB, result.Value);
		}

		bool CDB_Flagged = false;
		for (size_t i = 0; i < alu_units.size(); i++)
		{
			CDB_Flagged = alu_units[i].get_finished_flag();
			if (CDB_Flagged)
			{
				auto CBD_ROB = alu_units[i].get_ROB();
				auto CBD_RES = alu_units[i].get_result();
				ROB.common_data_bus(CBD_ROB, CBD_RES);
				URS.common_data_bus(CBD_ROB, CBD_RES);

			}
		}

		if (agu_CDB_flagged)
		{
			auto CBD_ROB = agu.get_ROB();
			auto CBD_RES = agu.get_result();
			ROB.common_data_bus(CBD_ROB, CBD_RES);
			URS.common_data_bus(CBD_ROB, CBD_RES);
		}
	}



	ROB.clock_tick();
	for (size_t i = 0; i < alu_units.size(); i++)
	{
		alu_units[i].clock_tick();
	}
	BU.clock_tick();
	URS.clock_tick();
	fet_unit.clock_tick();
	IIU.clock_tick();
	RAT.clock_tick();
	agu.clock_tick();
	MO.clock_tick();
	for (auto& x : registers)
	{
		x.clock_tick();

	}
}

processor_pipelined::processor_pipelined(const std::vector<int64_t> memory_state)
{
	test_memory.resize(1024);
	std::fill(test_memory.begin(), test_memory.end(), 0);
	if (test_memory.size() < memory_state.size()) { test_memory.resize(memory_state.size()); }
	std::copy(memory_state.begin(), memory_state.end(), test_memory.begin());
}

int64_t processor_pipelined::return_register_value(register_names reg)
{
	return registers[reg].get_register_value();
}


void processor_pipelined::print(std::ostream & os)
{
	for (auto& reg : registers)
	{
		reg.print(os);
	}
}

const std::vector<int64_t>& processor_pipelined::return_memory()
{
	return test_memory;//memory.return_memory();
}

int processor_pipelined::return_idle_time()
{
	return 0;
}

int processor_pipelined::return_executed_instructions()
{
	return static_cast<int>(ROB.get_ops_performed());

}

int processor_pipelined::return_clock_time()
{
	return  static_cast<int>(clock_ticks);
}
