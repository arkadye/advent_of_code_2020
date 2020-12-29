#include "../advent/advent3.h"

#include "../utils/advent_utils.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/Coords.h"

#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <sstream>
#include <algorithm>

namespace
{
	using Line = std::string;
	using Map = std::vector<Line>;
	using utils::Coords;

	constexpr char EMPTY = '.';
	constexpr char TREE = '#';

	Map stream_to_map(std::istream& input)
	{
		auto clean_input = [](std::string s)
		{
			const auto find_result = s.find(' ');
			return s.substr(0, find_result);
		};
		Map result;
		std::transform(utils::istream_line_iterator(input), utils::istream_line_iterator(), std::back_inserter(result), clean_input);
		assert(!result.empty());
		for (const Line& l : result)
		{
			assert(l.size() == result[0].size());
		}
		return result;
	}

	Map get_testcase_a_input()
	{
		std::istringstream input{
			std::string{	"..##.........##.........##.........##.........##.........##.......  --->\n"
							"#...#...#..#...#...#..#...#...#..#...#...#..#...#...#..#...#...#..\n"
							".#....#..#..#....#..#..#....#..#..#....#..#..#....#..#..#....#..#.\n"
							"..#.#...#.#..#.#...#.#..#.#...#.#..#.#...#.#..#.#...#.#..#.#...#.#\n"
							".#...##..#..#...##..#..#...##..#..#...##..#..#...##..#..#...##..#.\n"
							"..#.##.......#.##.......#.##.......#.##.......#.##.......#.##.....  --->\n"
							".#.#.#....#.#.#.#....#.#.#.#....#.#.#.#....#.#.#.#....#.#.#.#....#\n"
							".#........#.#........#.#........#.#........#.#........#.#........#\n"
							"#.##...#...#.##...#...#.##...#...#.##...#...#.##...#...#.##...#...\n"
							"#...##....##...##....##...##....##...##....##...##....##...##....#\n"
							".#..#...#.#.#..#...#.#.#..#...#.#.#..#...#.#.#..#...#.#.#..#...#.#  --->"}
		};
		return stream_to_map(input);
	}

	Map get_puzzle_input()
	{
		auto input = utils::open_puzzle_input(3);
		return stream_to_map(input);
	}

	int solve_p1_generic(const Map& map, int x_offset, int y_offset)
	{
		int current_x = 0;
		int current_y = 0;
		int num_trees = 0;
		while (current_y < static_cast<int>(map.size()))
		{
			const Line& line = map[current_y];
			const bool is_tree = (line[current_x % line.size()] == TREE);
			if (is_tree) ++num_trees;
			current_x += x_offset;
			current_y += y_offset;
		}
		return num_trees;
	}

	ResultType solve_p1(const Map& map)
	{
		return solve_p1_generic(map, 3, 1);
	}

	ResultType solve_p2(const Map& map)
	{
		const std::vector<Coords> paths
		{
			Coords{1,1},
			Coords{3,1},
			Coords{5,1},
			Coords{7,1},
			Coords{1,2}
		};

		std::vector<int> results;
		results.reserve(paths.size());
		auto solve_path = [&map](const Coords& c)
		{
			return solve_p1_generic(map, c.x, c.y);
		};

		std::transform(begin(paths), end(paths), std::back_inserter(results), solve_path);
		const int result = std::reduce(begin(results), end(results), 1, std::multiplies<int>());
		return result;
	}
}

ResultType day_three_p1_testcase_a()
{
	return solve_p1(get_testcase_a_input());
}

ResultType day_three_p2_testcase_a()
{
	return solve_p2(get_testcase_a_input());
}

ResultType advent_three_p1()
{
	return solve_p1(get_puzzle_input());
}

ResultType advent_three_p2()
{
	return solve_p2(get_puzzle_input());
}