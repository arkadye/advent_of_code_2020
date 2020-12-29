#pragma once

#include <fstream>
#include <sstream>
#include <cassert>

namespace utils
{

	inline std::ifstream open_puzzle_input(int day)
	{
		std::ostringstream name;
		name << "inputs/advent" << day << ".txt";
		auto result = std::ifstream{ name.str() };
		assert(result.is_open());
		return result;
	}

	inline std::ifstream open_testcase_input(int day, char id)
	{
		std::ostringstream name;
		name << "inputs/advent" << day << "_testcase_" << id << ".txt";
		auto result = std::ifstream{ name.str() };
		assert(result.is_open());
		return result;
	}
}
