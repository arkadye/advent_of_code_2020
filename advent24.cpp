#include "../advent/advent24.h"
#include "../utils/Coords.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/advent_utils.h"
#include "../utils/int_range.h"
#include "../utils/in_range.h"

#include <string>
#include <string_view>
#include <array>
#include <set>
#include <map>
#include <algorithm>
#include <vector>

namespace
{
	using utils::Coords;
	using Floor = std::set<Coords>;

	bool is_odd(int in)
	{
		return in % 2;
	}

	bool is_even(int in)
	{
		return !is_odd(in);
	}

	enum class Direction : char
	{
		east,
		northeast,
		northwest,
		southeast,
		southwest,
		west
	};

/*
*  HOW TO USE A SQUARE GRID AS A HEX GRID
* Y-coord: list of x-coords, with gaps to show how they match up
*
*  y=+2 | -3  -2  -1  00  +1  +2  +3
*  y=+1 |   -3  -2  -1  00  +1  +2
*  y= 0 | -3  -2  -1  00  +1  +2  +3
*  y=-1 |   -3  -2  -1  00  +1  +2
*  y=-2 | -3  -2  -1  00  +1  +2  +3
*
* y = +north -south
* x = +east -west
* So our movements are:
*	EAST: x+=1
*	WEST: x-=1
*	NORTHEAST: If odd(y) -> x+=1 ; y+=1
*	SOUTHEAST: If odd(y) -> x+=1 ; y-=1
*	NORTHWEST: If even(y) -> x-=1 ; y+=1
*	SOUTHWEST: If even(y) -> x-=1 ; y-=1
*/

	Coords move(Coords location, Direction dir)
	{
		auto& x = location.x;
		auto& y = location.y;
		switch (dir)
		{
		case Direction::east:
			++x;
			break;
		case Direction::west:
			--x;
			break;
		case Direction::northeast:
			if (is_odd(y)) ++x;
			++y;
			break;
		case Direction::southeast:
			if (is_odd(y)) ++x;
			--y;
			break;
		case Direction::northwest:
			if (is_even(y)) --x;
			++y;
			break;
		case Direction::southwest:
			if (is_even(y)) --x;
			--y;
			break;
		default:
			assert(false);
			break;
		}
		return location;
	}

	std::pair<std::string_view, Direction> get_direction(std::string_view input)
	{
		assert(!input.empty());
		using StrToDir = std::pair<std::string_view, Direction>;
		constexpr std::array<StrToDir, 6> direction_map {
			StrToDir{ "e", Direction::east},
			StrToDir{ "w",Direction::west },
			StrToDir{ "se",Direction::southeast },
			StrToDir{ "sw",Direction::southwest },
			StrToDir{ "ne",Direction::northeast },
			StrToDir{ "nw",Direction::northwest }
		};

		for (const auto& [str,result] : direction_map)
		{
			if (input.starts_with(str))
			{
				input.remove_prefix(str.size());
				return std::make_pair(input, result);
			}
		}
		assert(false);
		return std::pair("", Direction::east);  
	}

	Coords get_tile_to_flip(std::string_view line)
	{
		Coords location{ 0,0 };
		while (!line.empty())
		{
			const auto [remaining_line, dir] = get_direction(line);
			location = move(location, dir);
			line = remaining_line;
		}
		return location;
	}

	Floor get_black_tiles(std::istream& input)
	{
		using ItType = utils::istream_line_iterator;
		Floor black_tiles;
		for (ItType it{ input }; it != ItType{}; ++it)
		{
			const Coords tile_to_flip = get_tile_to_flip(*it);
			const auto prev_loc = black_tiles.find(tile_to_flip);
			if (prev_loc == end(black_tiles))
			{
				black_tiles.insert(tile_to_flip);
			}
			else
			{
				black_tiles.erase(prev_loc);
			}
		}
		return black_tiles;
	}

	std::array<Coords, 6> calculate_neighbours(const Coords& coords)
	{
		std::array<Coords, 6> result;
		result[0] = move(coords, Direction::east);
		result[1] = move(coords, Direction::west);
		result[2] = move(coords, Direction::northeast);
		result[3] = move(coords, Direction::northwest);
		result[4] = move(coords, Direction::southeast);
		result[5] = move(coords, Direction::southwest);
		return result;
	}

	std::vector<Coords> get_relevant_tiles(const Floor& floor)
	{
		std::vector<Coords> relevant_tiles;
		relevant_tiles.reserve(7 * floor.size());
		for (const Coords& c : floor)
		{
			relevant_tiles.push_back(c);
			const auto neighbours = calculate_neighbours(c);
			std::copy(begin(neighbours), end(neighbours), std::back_inserter(relevant_tiles));
		}
		std::sort(begin(relevant_tiles), end(relevant_tiles));
		const auto unique_result = std::unique(begin(relevant_tiles), end(relevant_tiles));
		relevant_tiles.erase(unique_result, end(relevant_tiles));
		return relevant_tiles;
	}

	bool is_tile_black(const Floor& floor, const Coords& tile)
	{
		return floor.contains(tile);
	}

	auto count_black_neighbours(const Floor& floor, const Coords& tile)
	{
		const auto neighbours = calculate_neighbours(tile);
		return std::count_if(begin(neighbours), end(neighbours),
			[&floor](const Coords& n) {return is_tile_black(floor, n); });
	}

	bool should_tile_be_black(const Floor& floor, const Coords& tile)
	{
		const auto num_neighbours = count_black_neighbours(floor, tile);
		return is_tile_black(floor, tile) ? utils::in_range(num_neighbours, 1, 2) : num_neighbours == 2;
	}

	Floor flip_tiles_overnight(const Floor& floor)
	{
		const auto tiles_to_check = get_relevant_tiles(floor);
		Floor result;
		std::copy_if(begin(tiles_to_check), end(tiles_to_check),std::inserter(result,begin(result)),
			[&floor](const Coords& tile) {return should_tile_be_black(floor, tile); });
		return result;
	}

	std::size_t solve_p1(std::istream& input)
	{
		const auto result = get_black_tiles(input);
		return result.size();
	}

	std::size_t solve_p2(std::istream& input, int num_iterations)
	{
		assert(num_iterations > 0);
		Floor floor = get_black_tiles(input);
		for (auto i : utils::int_range(num_iterations))
		{
			floor = flip_tiles_overnight(floor);
		}
		return floor.size();
	}
}

ResultType day_twentyfour_testcase_a()
{
	auto input = utils::open_testcase_input(24, 'a');
	return solve_p1(input);
}

ResultType advent_twentyfour_p1()
{
	auto input = utils::open_puzzle_input(24);
	return solve_p1(input);
}

ResultType day_twentyfour_testcase_b_generic(int num_iterations)
{
	auto input = utils::open_testcase_input(24, 'a');
	return solve_p2(input, num_iterations);
}

ResultType advent_twentyfour_p2()
{
	auto input = utils::open_puzzle_input(24);
	return solve_p2(input, 100);
}