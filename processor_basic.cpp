

#include "processor_basic.h"
#include "Instructions.h"

void processor_basic::clock_tick()
{
	++clock_ticks;
	auto status_value = registers[register_names::status].get_register_value();
	if (status_value & status_register_flags::memory_wait)
	{
		++idle_time;
	}
	else if (stage == fetch_stage)
	{
		fetch_instruction();

	}
	else if (stage == decode_stage)
	{

		instruction x{ registers[instruction_register].get_register_value() };
		decode_instruction(x);

		stage = execute_stage;
	}
	else if (stage == execute_stage)
	{
		++executed_instructions;
		registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() & ~(memory_op_fin^memory_wait^memory_read^memory_write));//reset status flags
		instruction x{ registers[instruction_register].get_register_value() };
		execute_instruction(x);
		stage = memory_access;

	}
	else if (stage == memory_access)
	{
		instruction x{ registers[instruction_register].get_register_value() };
		memory_access_instruction(x);
		stage = write_back;
	}
	else if (stage == write_back)
	{
		instruction x{ registers[instruction_register].get_register_value() };
		writeback_instruction(x);
		stage = fetch_stage;
	}

	memory.clock_tick();
	for (auto& x : registers)
	{
		x.clock_tick();

	}
}


processor_basic::processor_basic(const std::vector<int64_t> memory_state):memory(memory_state,registers.at(memory_address), registers.at(memory_buffer), registers.at(status)),test_memory(memory_state)
{

}

int64_t processor_basic::return_register_value(register_names reg)
{
	return registers[reg].get_register_value();
}


void processor_basic::print(std::ostream & os)
{
	for(auto& reg : registers )
	{
		reg.print(os);
	}
}

const std::vector<int64_t>& processor_basic::return_memory()
{
	return memory.return_memory();
}

int processor_basic::return_idle_time()
{
	return idle_time;
}

int processor_basic::return_executed_instructions()
{
	return executed_instructions;
}

int processor_basic::return_clock_time()
{
	return clock_ticks;
}

void processor_basic::fetch_instruction()
{
	if (!(registers[register_names::status].get_register_value() & status_register_flags::memory_op_fin))
	{
		registers[register_names::memory_address].set_register_value(registers[register_names::program_counter].get_register_value());
		registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() ^ (memory_read ^ memory_wait));
	}
	else
	{
		registers[register_names::instruction_register].set_register_value(registers[register_names::memory_buffer].get_register_value());
		registers[register_names::program_counter].set_register_value(registers[register_names::program_counter].get_register_value() + 1);
		registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() & ~(memory_op_fin^memory_wait^memory_read^memory_write));
		stage = decode_stage;
	}
}

void processor_basic::execute_instruction(instruction i)
{
	int64_t result = 0;
	int64_t r1 = registers[buffer_op1].get_register_value();
	int64_t r2 = registers[buffer_op2].get_register_value();
	switch (registers[buffer_opcode].get_register_value())
	{
	case instructions::NOP:
	case instructions::STO:
		break;
	case instructions::ADDR:
	case instructions::ADDI:
		result = r1 + r2;
		break;
	case instructions::MULR:
	case instructions::MULI:
		result = r1 * r2;
		break;
	case instructions::CMPR:
	case instructions::CMPI:
		result = r1 == r2 ? 0 : r1 > r2 ? 1 : -1;
		break;
	case instructions::LDR:
	case instructions::LDRR:
		result = r1 + r2;

		break;
	case instructions::LDC:
		result = i.get_operand_1();
		break;
	case instructions::BRI:
	case instructions::BRR:

		result = (r1 == r2) ? registers[buffer_destination].get_register_value() : registers[program_counter].get_register_value();
		break;
	case instructions::JMP:
		result = i.get_operand_1();
		break;
	case instructions::JMPR:
		result = r1 + r2;
		break;

	default:
		break;
	}
	registers[accumulator].set_register_value(result);


}

void processor_basic::decode_instruction(instruction x)
{
	registers[register_names::buffer_opcode].set_register_value(x.get_opcode());

	switch (x.get_opcode())
	{
		case instructions::NOP://Ignore dest, op1 and op2.
			break;
		case instructions::ADDR://all ops set as needed
		case instructions::MULR://all ops set as needed
		case instructions::CMPR://all ops set as needed
		case instructions::LDRR://all ops set as needed
		case instructions::STOR:
			registers[buffer_destination].set_register_value(x.get_destination());
			registers[buffer_op1].set_register_value(registers[x.get_operand_1()].get_register_value());
			registers[buffer_op2].set_register_value(registers[x.get_operand_2()].get_register_value());
			break;
		case instructions::ADDI:
		case instructions::MULI:
		case instructions::CMPI:
		case instructions::LDR:
		case instructions::STO:
			registers[buffer_destination].set_register_value(x.get_destination());
			registers[buffer_op1].set_register_value(registers[x.get_operand_1()].get_register_value());
			registers[buffer_op2].set_register_value(x.get_operand_2());
			break;
		case instructions::BRR:
			registers[buffer_destination].set_register_value(registers[x.get_destination()].get_register_value());
			registers[buffer_op1].set_register_value(registers[x.get_operand_1()].get_register_value());
			registers[buffer_op2].set_register_value(registers[x.get_operand_2()].get_register_value());
			break;
		case instructions::BRI:
			registers[buffer_destination].set_register_value(registers[x.get_destination()].get_register_value());
			registers[buffer_op1].set_register_value(registers[x.get_operand_1()].get_register_value());
			registers[buffer_op2].set_register_value(x.get_operand_2());
			break;
		case instructions::LDC:
			registers[buffer_destination].set_register_value(x.get_destination());
			registers[buffer_op1].set_register_value(x.get_operand_1());
			break;
		case instructions::JMPR:
			registers[buffer_destination].set_register_value(program_counter);
			registers[buffer_op1].set_register_value(registers[x.get_destination()].get_register_value());
			registers[buffer_op2].set_register_value(x.get_operand_2());
			break;
		case instructions::JMP:
			registers[buffer_destination].set_register_value(program_counter);
			registers[buffer_op1].set_register_value(x.get_operand_1());
			break;
		default:
			break;


	}

}

void processor_basic::memory_access_instruction(instruction x)
{
	switch (registers[buffer_opcode].get_register_value())
	{
	case instructions::LDR:
	case instructions::LDRR:
		registers[register_names::memory_address].set_register_value(registers[accumulator].get_register_value());
		registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() ^ (memory_read ^ memory_wait));
		break;
	case instructions::STO:
		registers[register_names::memory_buffer].set_register_value(registers[x.get_operand_1()].get_register_value());
		registers[register_names::memory_address].set_register_value(registers[x.get_destination()].get_register_value() + x.get_operand_2());
		registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() ^ (memory_write ^ memory_wait));
		break;
	default:
		break;
	}
}

void processor_basic::writeback_instruction(instruction x)
{
	int64_t t = registers[buffer_opcode].get_register_value();
	switch (t)
	{
	case NOP:
		break;
	case STO:
	case STOR:
		registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() & ~(memory_op_fin^memory_wait^memory_read^memory_write));
		break;
	case ADDR:
	case ADDI:
	case MULR:
	case MULI:
	case CMPR:
	case CMPI:
	case LDC:
		registers[registers[buffer_destination].get_register_value()].set_register_value(registers[accumulator].get_register_value());
		break;
	case LDR:
	case LDRR:
		registers[register_names::status].set_register_value(registers[register_names::status].get_register_value() & ~(memory_op_fin^memory_wait^memory_read^memory_write));
		registers[registers[buffer_destination].get_register_value()].set_register_value(registers[memory_buffer].get_register_value());
		break;
	case BRI:
	case BRR:
	case JMP:
	case JMPR:
		registers[program_counter].set_register_value(registers[accumulator].get_register_value());
		break;
	default:
		break;
	}
}
