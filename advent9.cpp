#include "../advent/advent9.h"
#include "../utils/advent_utils.h"
#include "../utils/int_range.h"

#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdint>

namespace
{
	using namespace utils;
	using FileIt = std::istream_iterator<int64_t>;
	std::ifstream get_testcase_a_input()
	{
		return open_testcase_input(9, 'a');
	}

	bool validate_value(const std::vector<int64_t>& preamble, int64_t value)
	{
		for (auto first_i : int_range(preamble.size()))
		{
			const auto first_val = preamble[first_i];
			for (auto second_i : int_range(first_i + 1, preamble.size()))
			{
				assert(first_i < second_i);
				const auto second_val = preamble[second_i];
				if (first_val + second_val == value)
				{
					return true;
				}
			}
		}
		return false;
	}

	struct FileAndBadValue
	{
		std::vector<int64_t> all_vals;
		int64_t bad_val;
	};

	FileAndBadValue get_file_and_bad_value(std::ifstream& input, std::size_t preamble_size, bool get_all_values)
	{
		std::vector<int64_t> preamble;
		FileAndBadValue result;
		preamble.reserve(preamble_size);
		for (auto i : int_range(preamble_size))
		{
			int64_t val = 0;
			input >> val;
			preamble.push_back(val);
			if (get_all_values)
			{
				result.all_vals.push_back(val);
			}
		}

		std::size_t oldest = 0;

		while (!input.eof())
		{
			int64_t test_val = 0;
			input >> test_val;
			if (!validate_value(preamble, test_val))
			{
				result.bad_val = test_val;
				return result;
			}

			if (get_all_values)
			{
				result.all_vals.push_back(test_val);
			}
			preamble[oldest++] = test_val;
			if (oldest == preamble_size)
			{
				oldest = 0;
			}
		}
		assert(false); // We should always find a one.
		return result;
	}

	int64_t solve_p1(std::ifstream& input, std::size_t preamble_size)
	{
		return get_file_and_bad_value(input, preamble_size, false).bad_val;
	}

	int64_t solve_p2(std::ifstream& input, std::size_t preamble_size)
	{
		const FileAndBadValue data = get_file_and_bad_value(input, preamble_size, true);
		const std::vector<int64_t>& values = data.all_vals;
		std::size_t first = 0;
		std::size_t last = 1;
		int64_t sum = values[first] + values[last];
		while (sum != data.bad_val)
		{
			if (sum < data.bad_val)
			{
				sum += values[++last];
			}
			else if (sum > data.bad_val)
			{
				sum -= values[first++];
			}
		}
		assert(first < last);
		assert(first < values.size());
		assert(last < values.size());
		const auto results = std::minmax_element(begin(values) + first, begin(values) + last);
		return *results.first + *results.second;
	}
}

ResultType day_nine_testcase_a()
{
	std::ifstream input = get_testcase_a_input();
	return solve_p1(input, 5);
}

ResultType day_nine_testcase_b()
{
	std::ifstream input = get_testcase_a_input();
	return solve_p2(input, 5);
}

ResultType advent_nine_p1()
{
	std::ifstream input = open_puzzle_input(9);
	return solve_p1(input, 25);
}

ResultType advent_nine_p2()
{
	std::ifstream input = open_puzzle_input(9);
	return solve_p2(input, 25);
}