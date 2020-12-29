#include "../advent/advent17.h"
#include "../utils/advent_utils.h"
#include "../utils/int_range.h"
#include "../utils/in_range.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/conway_simulation.h"

#include <set>
#include <array>
#include <algorithm>
#include <sstream>
#include <compare>

#ifndef NDEBUG
#define DEBUG_OUT 0
#else
#define DEBUG_OUT 0
#endif

#if DEBUG_OUT
#include <iostream>
#endif

namespace
{
	using utils::int_range;
	using utils::istream_line_iterator;
	using utils::conway_simulation;

	template <std::size_t DIM>
	using PointData = typename conway_simulation<DIM>::PointData;
	template <std::size_t DIM>
	using Coord = typename conway_simulation<DIM>::Coord;
	template <std::size_t DIM>
	using CoordType = typename conway_simulation<DIM>::CoordType;

	std::istringstream get_testcase_input()
	{
		return std::istringstream{
			".#.\n"
			"..#\n"
			"###"
		};
	}

	template <std::size_t DIM>
	PointData<DIM> extract_line(std::string_view line, CoordType<DIM> line_num)
	{
		PointData<DIM> result;
		for (auto line_position : int_range(static_cast<CoordType<DIM>>(line.size())))
		{
			const char icon = line[line_position];
			if (icon == '#')
			{
				Coord<DIM> active_point;
				std::fill(begin(active_point), end(active_point), 0);
				assert(active_point.size() > 2);
				active_point[0] = line_position;
				active_point[1] = line_num;
				result.push_back(active_point);
			}
			else
			{
				assert(icon == '.');
			}
		}
		return result;
	}

	template <std::size_t DIM>
	conway_simulation<DIM> extract_initial_state(std::istream& input)
	{
		PointData<DIM> result;
		CoordType<DIM> line_num = 0;
		auto process_line = [&result, &line_num](std::string_view line)
		{
			auto line_result = extract_line<DIM>(line, line_num++);
			std::copy(begin(line_result), end(line_result), std::back_inserter(result));
		};
		std::for_each(istream_line_iterator{ input }, istream_line_iterator{}, process_line);
		return conway_simulation<DIM>{std::move(result), 3, 3, 2, 3};
	}

	template <std::size_t DIM>
	std::size_t solve_generic(std::istream& input)
	{
		auto state = extract_initial_state<DIM>(input);
		for (auto i : int_range(6))
		{
			state.tick();
		}
		return state.get_active_points().size();
	}

	std::size_t solve_p1(std::istream& input)
	{
		return solve_generic<3>(input);
	}

	std::size_t solve_p2(std::istream& input)
	{
		return solve_generic<4>(input);
	}
}

ResultType day_seventeen_testcase_a()
{
	auto input = get_testcase_input();
	return solve_p1(input);
}

ResultType advent_seventeen_p1()
{
	auto input = utils::open_puzzle_input(17);
	return solve_p1(input);
}

ResultType day_seventeen_testcase_b()
{
	auto input = get_testcase_input();
	return solve_p2(input);
}

ResultType advent_seventeen_p2()
{
	auto input = utils::open_puzzle_input(17);
	return solve_p2(input);
}

#undef DEBUG_OUT