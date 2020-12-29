#include "../advent/advent1.h"

#include "../utils/sorted_vector.h"
#include "../utils/advent_utils.h"

#include <sstream>
#include <fstream>
#include <optional>
#include <algorithm>
#include <cassert>
#include <iterator>

// Unnamed namespaces are really useful for functions
// specific to this solution. Generally useful stuff should
// go into a utility header. (Some stuff is already there.)
namespace
{
	using ValList = utils::sorted_vector<uint16_t>;

	template <typename SortedIt>
	std::optional<int> get_value_combination(SortedIt start, SortedIt finish, int target, int num_values)
	{
		assert(num_values > 0);
		// End condition
		if (num_values == 1)
		{
			if (std::binary_search(start, finish, target))
			{
				return target;
			}
			else
			{
				return std::nullopt;
			}
		}
		for (SortedIt it = start; it != finish; ++it)
		{
			const int candidate = *it;
			const int number_needed = target - candidate;
			if (candidate > number_needed) // Won't find anything
			{
				return std::nullopt;
			}
			const auto sub_result = get_value_combination(start + 1, finish, number_needed, num_values - 1);
			if (sub_result.has_value())
			{
				return sub_result.value() * candidate;
			}
		}
		
		// If we fell out of the loop, there's nothing here.
		return std::nullopt;
	}

	ResultType solve_p1_general(const ValList& data)
	{
		const auto result = get_value_combination(begin(data),end(data), 2020, 2);
		assert(result.has_value());
		return result.value();
	}

	ResultType solve_p2_general(const ValList& data)
	{
		const auto result = get_value_combination(begin(data), end(data), 2020, 3);
		assert(result.has_value());
		return result.value();
	}

	utils::sorted_vector<uint16_t> make_from_stream(std::istream& input, int max)
	{
		return utils::sorted_vector<uint16_t>{std::istream_iterator<uint16_t>(input), std::istream_iterator<uint16_t>()};
	}

	ValList get_testcase_input(int max_value)
	{
		auto data = std::istringstream{ "1721\n"
									"979\n"
									"366\n"
									"299\n"
									"675\n"
									"1456"
		};
		return make_from_stream(data, max_value);
	}

	ValList get_puzzle_input(int max_value)
	{
		auto data = utils::open_puzzle_input(1);
		return make_from_stream(data, max_value);
	}
}

ResultType day_one_testcase_a()
{
	return solve_p1_general(get_testcase_input(2020));
}

ResultType day_one_testcase_b()
{
	return solve_p2_general(get_testcase_input(2020));
}

ResultType advent_one_p1()
{
	return solve_p1_general(get_puzzle_input(2020));
}

ResultType advent_one_p2()
{
	return solve_p2_general(get_puzzle_input(2020));
}