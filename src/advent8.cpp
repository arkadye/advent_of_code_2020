#include "../advent/advent8.h"
#include "../utils/advent_utils.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/int_range.h"

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <functional>
#include <numeric>
#include <execution>

namespace
{
	using namespace utils;
	enum class Opcode : char
	{
		acc,
		jmp,
		nop,
		INVALID
	};

	Opcode to_opcode(const std::string& str)
	{
		static const std::map<std::string, Opcode> codes{
			{"acc", Opcode::acc},
			{"jmp", Opcode::jmp},
			{"nop",Opcode::nop}
		};
		const auto find_result = codes.find(str);
		assert(find_result != end(codes));
		return find_result != end(codes) ? find_result->second : Opcode::INVALID;
	}

	struct Instruction
	{
		int argument = 0;
		Opcode op = Opcode::INVALID;
	};

	Instruction to_instruction(std::string input)
	{
		std::istringstream iss{ std::move(input) };
		std::string op_str;
		std::string arg_str;
		iss >> op_str >> arg_str;

		return Instruction{ std::stoi(arg_str), to_opcode(op_str) };
	}

	using Program = std::vector<Instruction>;

	Program extract_program(std::istream& input)
	{
		Program result;
		std::transform(istream_line_iterator(input), istream_line_iterator(),
			std::back_inserter(result), to_instruction);
		return result;
	}

	Program get_basic_program()
	{
		return Program{ Instruction{0,Opcode::jmp} };
	}

	class HandheldConsoleCPU
	{
		Program m_code;
		std::size_t m_ip = 0;
		int m_accumulator = 0;

		using ExecuteInstruction = std::function<void(int)>;
	public:
		explicit HandheldConsoleCPU(Program program) : m_code{ std::move(program) } {}
		HandheldConsoleCPU() : HandheldConsoleCPU{ get_basic_program() } {}
		std::size_t get_ip() const { return m_ip; }
		int get_acc() const { return m_accumulator; }
		bool terminated() const { return m_ip >= m_code.size(); }
		void execute_next_instruction()
		{
			if (terminated())
			{
				return;
			}

			const Instruction& instruction = m_code[m_ip++];
			const std::map<Opcode, ExecuteInstruction> executors
			{
				{Opcode::acc,[this](int arg) { m_accumulator += arg; }},
				{Opcode::jmp,[this](int arg) {m_ip += (arg - 1); }},
				{Opcode::nop,[](int) {}},
				{Opcode::INVALID,[](int) {assert(false); } }
			};
			const auto ex_it = executors.find(instruction.op);
			assert(ex_it != end(executors));
			if (ex_it != end(executors))
			{
				ex_it->second(instruction.argument);
			}
			return;
		}
	};

	HandheldConsoleCPU run_till_loop_found_or_terminates(Program program)
	{
		HandheldConsoleCPU cpu{ std::move(program) };
		std::set<std::size_t> visited_points;
		while (!cpu.terminated() &&	 visited_points.find(cpu.get_ip()) == end(visited_points))
		{
			visited_points.insert(cpu.get_ip());
			cpu.execute_next_instruction();
		}
		return cpu;
	}

	HandheldConsoleCPU flip_instruction_and_run(Program program, std::size_t location_to_change)
	{
		Instruction& instruction = program[location_to_change];

		switch (instruction.op)
		{
		case Opcode::jmp:
			instruction.op = Opcode::nop;
			break;
		case Opcode::nop:
			instruction.op = Opcode::jmp;
			break;
		default:
			return HandheldConsoleCPU{ Program{Instruction{0,Opcode::jmp} } };
		}

		return run_till_loop_found_or_terminates(std::move(program));
	}

	int solve_p1(std::istream& code)
	{
		return run_till_loop_found_or_terminates(extract_program(code)).get_acc();
	}

	HandheldConsoleCPU find_one_that_terminated(const HandheldConsoleCPU& a, const HandheldConsoleCPU& b)
	{
		return a.terminated() ? a : b;
	}

	int solve_p2(std::istream& input)
	{
		const Program program = extract_program(input);
		int_range range{ program.size() };
		const auto result = std::transform_reduce(std::execution::parallel_unsequenced_policy{},
			begin(range), end(range),
			HandheldConsoleCPU{}, find_one_that_terminated, [&program](std::size_t i)
		{
			return flip_instruction_and_run(program, i);
		});
		return result.get_acc();
	}

	std::string get_testcase_a_input()
	{
		return "nop +0\n"
			"acc +1\n"
			"jmp +4\n"
			"acc +3\n"
			"jmp -3\n"
			"acc -99\n"
			"acc +1\n"
			"jmp -4\n"
			"acc +6";
	}
}

ResultType day_eight_testcase_a()
{
	std::istringstream input{ get_testcase_a_input() };
	return solve_p1(input);
}

ResultType advent_eight_p1()
{
	std::ifstream input = open_puzzle_input(8);
	return solve_p1(input);
}

ResultType day_eight_testcase_b()
{
	std::istringstream input{ get_testcase_a_input() };
	return solve_p2(input);
}

ResultType advent_eight_p2()
{
	std::ifstream input = open_puzzle_input(8);
	return solve_p2(input);
}
