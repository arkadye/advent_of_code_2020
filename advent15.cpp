#include "../advent/advent15.h"

#include <vector>
#include <cassert>
#include <algorithm>

namespace
{
	using ValueType = std::size_t;
	using GameState = std::vector<ValueType>;

	const GameState testcase_a{ 0,3,6 };
	const GameState testcase_b{ 1,3,2 };
	const GameState testcase_c{ 2,1,3 };
	const GameState testcase_d{ 1,2,3 };
	const GameState testcase_e{ 2,3,1 };
	const GameState testcase_f{ 3,2,1 };
	const GameState testcase_g{ 3,1,2 };
	const GameState puzzle_input{ 0,13,1,8,6,15 };

	ValueType get_nth_value(const GameState& initial, std::size_t n)
	{
		assert(!initial.empty());
		constexpr auto NOT_SEEN = std::numeric_limits<std::size_t>::max();
		std::vector<ValueType> game_data(n, NOT_SEEN);
		for (std::size_t i = 0; i < initial.size() - 1; ++i)
		{
			game_data[initial[i]] = i;
		}
		ValueType latest = initial.back();
		for (std::size_t i = initial.size(); i < n; ++i)
		{
			const auto last_seen = game_data[latest];
			const bool found = last_seen != NOT_SEEN;
			const ValueType location = found ? last_seen : 0;
			game_data[latest] = i - 1;
			latest = found ? i - location - 1 : 0;
		}
		return latest;
	}

	ValueType solve_p1(const GameState& initial)
	{
		return get_nth_value(initial, 2020);
	}

	ValueType solve_p2(const GameState& initial)
	{
		return get_nth_value(initial, 30'000'000);
	}
}

ResultType day_fifteen_testcase_a()
{
	return solve_p1(testcase_a);
}

ResultType day_fifteen_testcase_b()
{
	return solve_p1(testcase_b);
}

ResultType day_fifteen_testcase_c()
{
	return solve_p1(testcase_c);
}

ResultType day_fifteen_testcase_d()
{
	return solve_p1(testcase_d);
}

ResultType day_fifteen_testcase_e()
{
	return solve_p1(testcase_e);
}

ResultType day_fifteen_testcase_f()
{
	return solve_p1(testcase_f);
}

ResultType day_fifteen_testcase_g()
{
	return solve_p1(testcase_g);
}

ResultType day_fifteen_testcase_h()
{
	return solve_p2(testcase_a);
}

ResultType day_fifteen_testcase_i()
{
	return solve_p2(testcase_b);
}

ResultType day_fifteen_testcase_j()
{
	return solve_p2(testcase_c);
}

ResultType day_fifteen_testcase_k()
{
	return solve_p2(testcase_d);
}

ResultType day_fifteen_testcase_l()
{
	return solve_p2(testcase_e);
}

ResultType day_fifteen_testcase_m()
{
	return solve_p2(testcase_f);
}

ResultType day_fifteen_testcase_n()
{
	return solve_p2(testcase_g);
}

ResultType advent_fifteen_p1()
{
	return solve_p1(puzzle_input);
}

ResultType advent_fifteen_p2()
{
	return solve_p2(puzzle_input);
}