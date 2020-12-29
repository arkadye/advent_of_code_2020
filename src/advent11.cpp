#include "../advent/advent11.h"
#include "../utils/advent_utils.h"
#include "../utils/Coords.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/int_range.h"
#include "../utils/in_range.h"

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

namespace
{
	using namespace utils;
	enum class Space : char
	{
		empty_seat = 'L',
		occupied_seat = '#',
		floor = '.'
	};

	Space to_space(char c)
	{
		assert(std::string("L#.").find(c) != std::string::npos);
		return static_cast<Space>(c);
	}

	char to_char(Space s)
	{
		return static_cast<char>(s);
	}

	struct SpaceData
	{
		Space current = Space::floor;
		Space next = Space::floor;
	};

	struct Layout
	{
		std::vector<SpaceData> space;
		std::vector<std::size_t> chair_indices;
		std::size_t width = 0;
	};

	Layout get_layout(std::istream& input)
	{
		Layout result;
		for (auto line_it = istream_line_iterator(input); line_it != istream_line_iterator(); ++line_it)
		{
			std::string line = *line_it;
			assert(result.width == 0 || result.width == line.size());
			result.width = line.size();
			std::transform(begin(line), end(line), std::back_inserter(result.space),
				[](char c) {return SpaceData{ to_space(c), to_space(c) }; });
		}

		for (auto i : int_range(result.space.size()))
		{
			if (result.space[i].current != Space::floor)
			{
				result.chair_indices.push_back(i);
			}
		}
		return result;
	}

	Coords get_limit(const Layout& layout)
	{
		return Coords{ static_cast<int>(layout.width) , static_cast<int>(layout.space.size() / layout.width) };
	}

	std::size_t get_cell_index(const Layout& layout, const Coords& loc)
	{
		if (!in_range<int>(loc.x, 0, layout.width - 1)) return layout.space.max_size();
		if (!in_range<int>(loc.y, 0, layout.space.size() / layout.width - 1)) return layout.space.max_size();
		const std::size_t index = loc.y * layout.width + loc.x;
		return index;
	}

	Coords get_cell_coords(const Layout& layout, std::size_t index)
	{
		return Coords{ static_cast<int>(index % layout.width) , static_cast<int>(index / layout.width) };
	}

	Space get_contents(const Layout& layout, const Coords& loc)
	{
		const auto index = get_cell_index(layout, loc);
		return index < layout.space.size() ? layout.space[index].current : Space::empty_seat;
	}

	bool is_occupied(const Layout& layout, const Coords& loc)
	{
		return get_contents(layout,loc) == Space::occupied_seat;
	}

	enum class NeighbourBehaviour : bool
	{
		immediate,
		directional
	};

	Layout calculate_next_iteration(Layout layout, int sit_threshold, int stand_threshold, NeighbourBehaviour behaviour)
	{
		for (std::size_t cell_index : layout.chair_indices)
		{
			SpaceData& cell = layout.space[cell_index];
			const Coords& coords = get_cell_coords(layout, cell_index);
			const Space current = cell.current;
			Space& next = cell.next;
			next = current;
			assert(current == Space::empty_seat || current == Space::occupied_seat);
			const std::array<Coords, 8> directions {
				Coords::up(),
				Coords::up() + Coords::right(),
				Coords::right(),
				Coords::right() + Coords::down(),
				Coords::down(),
				Coords::down() + Coords::left(),
				Coords::left(),
				Coords::left() + Coords::up()
			};

			auto get_neighbour = [behaviour, &coords, &layout](const Coords& direction)
			{
				if (behaviour == NeighbourBehaviour::immediate)
				{
					return coords + direction;
				}

				const Coords limit = get_limit(layout);
				for (int i = 1; true; ++i)
				{
					const Coords test_coords = coords + i * direction;
					if (!in_range(test_coords.x, 0, limit.x)) return test_coords;
					if (!in_range(test_coords.y, 0, limit.y)) return test_coords;
					if (get_contents(layout, test_coords) != Space::floor) return test_coords;
				}
				assert(false);
				return Coords{};
			};

			std::array<Coords, 8> neighbours;
			std::transform(begin(directions), end(directions), begin(neighbours), get_neighbour);

			auto is_seat_occupied = [&layout](const Coords& c)
			{
				return is_occupied(layout, c);
			};

			const auto neighbours_occupied = std::count_if(begin(neighbours), end(neighbours), is_seat_occupied);
			if (neighbours_occupied <= sit_threshold)
			{
				next = Space::occupied_seat;
			}
			else if (neighbours_occupied >= stand_threshold)
			{
				next = Space::empty_seat;
			}
		}
		return layout;
	}

	Layout update_to_next_iteration(Layout layout)
	{
		for (auto& i : layout.chair_indices)
		{
			SpaceData& sd = layout.space[i];
			sd.current = sd.next;
		}
		return layout;
	}

	bool has_changes(const Layout& layout)
	{
		return std::any_of(begin(layout.chair_indices), end(layout.chair_indices),
			[&layout](std::size_t i) {
			const SpaceData data = layout.space[i];
			return data.current != data.next;
		});
	}

	void print_layout(const Layout& layout)
	{
#if 0
#ifndef NDEBUG
		for (auto i : int_range(layout.space.size()))
		{
			if (i % layout.width == 0) std::cout << '\n';
			std::cout << to_char(layout.space[i].current);
		}
		std::string dummy;
		std::getline(std::cin, dummy);
#endif
#endif
	}

	int solve_generic(std::istream& input, int sit_threshold, int stand_threshold, NeighbourBehaviour behaviour)
	{
		Layout layout = get_layout(input);
		while (true)
		{
			print_layout(layout);
			layout = calculate_next_iteration(std::move(layout),sit_threshold,stand_threshold,behaviour);
			if (!has_changes(layout))
			{
				break;
			}
			layout = update_to_next_iteration(std::move(layout));
		}
		return std::count_if(begin(layout.chair_indices), end(layout.chair_indices),
			[&layout](std::size_t i) {return layout.space[i].current == Space::occupied_seat; });
	}

	int solve_p1(std::istream& input)
	{
		return solve_generic(input, 0, 4, NeighbourBehaviour::immediate);
	}

	int solve_p2(std::istream& input)
	{
		return solve_generic(input, 0, 5, NeighbourBehaviour::directional);
	}
}

ResultType day_eleven_testcase_a()
{
	auto input = open_testcase_input(11, 'a');
	return solve_p1(input);
}

ResultType day_eleven_testcase_b()
{
	auto input = open_testcase_input(11, 'a');
	return solve_p2(input);
}

ResultType advent_eleven_p1()
{
	auto input = open_puzzle_input(11);
	return solve_p1(input);
}

ResultType advent_eleven_p2()
{
	auto input = open_puzzle_input(11);
	return solve_p2(input);
}
