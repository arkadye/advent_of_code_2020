#include "../advent/advent10.h"
#include "../utils/advent_utils.h"
#include "../utils/sorted_vector.h"
#include "../utils/int_range.h"

#include <algorithm>
#include <numeric>
#include <vector>

namespace
{
	using namespace utils;
	std::ifstream get_testcase_a()
	{
		return open_testcase_input(10, 'a');
	}

	std::ifstream get_testcase_b()
	{
		return open_testcase_input(10, 'b');
	}

	using Joltage = int;
	
	sorted_vector<Joltage> get_adaptor_list(std::istream& input)
	{
		using FileIt = std::istream_iterator<Joltage>;
		auto result = sorted_vector<Joltage>(FileIt{ input }, FileIt{});
		result.insert(0);
		result.insert(result.back() + 3);
		assert(!result.empty());
		return result;
	}

	int solve_p1(std::istream& input)
	{
		const auto data = get_adaptor_list(input);
		int three_diff_count = 0;
		int one_diff_count = 0;
		for (auto i : int_range(data.size() - 1))
		{
			const auto current = data[i];
			const auto next = data[i + 1];
			assert(next > current);
			const auto difference = next - current;
			switch (difference)
			{
			case 1:
				++one_diff_count;
				break;
			case 3:
				++three_diff_count;
				break;
			default:
				break;
			}
		}
		return one_diff_count * three_diff_count;
	}

	uint64_t solve_p2(std::istream& input)
	{
		const auto data = get_adaptor_list(input);
		struct Possibilities
		{
			uint64_t num_possibilities;
			std::size_t index;
			Joltage jolts;
		};

		std::vector<Possibilities> cache{ Possibilities{1,data.size() - 1,data.back()} };
		cache.reserve(4);
		for (auto x : int_range(data.size()-1))
		{
			const std::size_t index = data.size() - x - 2;
			const Joltage current_jolts = data[index];
			const auto remove_it = std::remove_if(begin(cache), end(cache),
				[current_jolts](const Possibilities& p) {return (p.jolts - current_jolts) > 3; });
			cache.erase(remove_it, end(cache));

			const uint64_t num_possibilities = std::accumulate(begin(cache), end(cache), uint64_t{ 0 },
				[](uint64_t total, const Possibilities& p) {return total + p.num_possibilities; });

			cache.push_back(Possibilities{ num_possibilities,index,current_jolts });
		}

		assert(cache.back().index == 0);
		return cache.back().num_possibilities;
	}
}

ResultType day_ten_testcase_a()
{
	auto input = open_testcase_input(10, 'a');
	return solve_p1(input);
}

ResultType day_ten_testcase_b()
{
	auto input = open_testcase_input(10, 'b');
	return solve_p1(input);
}

ResultType advent_ten_p1()
{
	auto input = open_puzzle_input(10);
	return solve_p1(input);
}

ResultType day_ten_testcase_c()
{
	auto input = open_testcase_input(10, 'a');
	return solve_p2(input);
}

ResultType day_ten_testcase_d()
{
	auto input = open_testcase_input(10, 'b');
	return solve_p2(input);
}

ResultType advent_ten_p2()
{
	auto input = open_puzzle_input(10);
	return solve_p2(input);
}
