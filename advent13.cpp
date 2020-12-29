#include "../advent/advent13.h"
#include "../utils/advent_utils.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/sorted_vector.h"

#include <algorithm>
#include <sstream>
#include <numeric>
#include <vector>

namespace
{
	using namespace utils;
	std::istringstream get_testcase_input_a()
	{
		return std::istringstream{ "939\n7,13,x,x,59,x,31,19" };
	}

	struct BusData
	{
		uint64_t id = 0;
		uint64_t wait_time = std::numeric_limits<uint64_t>::max();
	};

	uint64_t get_wait_time(uint64_t id, uint64_t start_time)
	{
		const auto cycles = start_time / id;
		auto leave_time = id * cycles;
		if (leave_time < start_time) leave_time += id;
		assert(leave_time >= start_time);
		return leave_time - start_time;
	}

	BusData get_bus_data(const std::string& bus_id, uint64_t start_time)
	{
		BusData result;
		if (bus_id != "x")
		{
			assert(!bus_id.empty());
			assert(std::all_of(begin(bus_id), end(bus_id), ::isdigit));
			result.id = std::stoi(bus_id);
			result.wait_time = get_wait_time(result.id, start_time);
		}
		return result;
	}

	BusData get_best_bus(const BusData& a, const BusData& b)
	{
		return a.wait_time < b.wait_time ? a : b;
	}

	BusData get_next_bus(std::istream& input, uint64_t start_time)
	{
		return std::transform_reduce(istream_line_iterator{ input,',' }, istream_line_iterator{},
			BusData{},
			get_best_bus,
			[start_time](const std::string& id) {return get_bus_data(id, start_time); });
	}

	uint64_t get_start_time(std::istream& input)
	{
		uint64_t result = 0;
		input >> result;
		input.ignore();
		return result;
	}

	uint64_t solve_p1(std::istream& input)
	{
		const uint64_t start_time = get_start_time(input);
		const BusData result = get_next_bus(input, start_time);
		return result.id * result.wait_time;
	}
}

ResultType day_thirteen_testcase_a()
{
	auto input = get_testcase_input_a();
	return solve_p1(input);
}

ResultType advent_thirteen_p1()
{
	auto input = open_puzzle_input(13);
	return solve_p1(input);
}

namespace
{
	sorted_vector<BusData> get_all_buses(std::istream& input)
	{
		get_start_time(input); // And discard it.
		sorted_vector<BusData> result([](const BusData& a, const BusData& b) {return a.id > b.id; });
		uint64_t mod = 0;
		for (auto it = istream_line_iterator{ input,',' }; it != istream_line_iterator{}; ++it)
		{
			BusData bd = get_bus_data(*it, 0);
			if (bd.id > 0)
			{
				bd.wait_time = mod % bd.id;
				result.insert(bd);
			}
			++mod;
		}
		assert(!result.empty());
		return result;
	}

	template <typename ItType>
	uint64_t solve_p2(ItType first, ItType last, uint64_t increment, uint64_t start_time)
	{
		++first;
		if (first == last)
		{
			return start_time;
		}

		const BusData& data = *first;
		while (true)
		{
			const auto wait_time = get_wait_time(data.id, start_time);
			if (wait_time == data.wait_time)
			{
				return solve_p2(first, last, increment * data.id, start_time);
			}
			start_time += increment;
		}
	}

	uint64_t solve_p2(const sorted_vector<BusData>& buses)
	{
		const uint64_t increment = buses.front().id;
		const uint64_t initial_test_value = increment - buses.front().wait_time;
		return solve_p2(begin(buses), end(buses), increment, initial_test_value);
	}

	uint64_t solve_p2(std::istream& input)
	{
		const auto bus_data = get_all_buses(input);
		return solve_p2(bus_data);
	}
}

ResultType day_thirteen_testcase_b()
{
	auto input = get_testcase_input_a();
	return solve_p2(input);
}

ResultType day_thirteen_testcase_c()
{
	std::istringstream input{ "0\n17,x,13,19" };
	return solve_p2(input);
}

ResultType day_thirteen_testcase_d()
{
	std::istringstream input{ "0\n67,7,59,61" };
	return solve_p2(input);
}

ResultType day_thirteen_testcase_e()

{
	std::istringstream input{ "0\n67,x,7,59,61" };
	return solve_p2(input);
}

ResultType day_thirteen_testcase_f()
{
	std::istringstream input{ "0\n67,7,x,59,61" };
	return solve_p2(input);
}

ResultType day_thirteen_testcase_g()
{
	std::istringstream input{ "0\n1789,37,47,1889" };
	return solve_p2(input);
}

ResultType advent_thirteen_p2()
{
	auto input = open_puzzle_input(13);
	return solve_p2(input);
}