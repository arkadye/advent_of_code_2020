#include "../advent/advent20.h"
#include "../utils/advent_utils.h"

#include "../utils/to_value.h"
#include "../utils/int_range.h"
#include "../utils/transform_if.h"
#include "../utils/isqrt.h"
#include "../utils/Coords.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/in_range.h"

#include <string>
#include <array>
#include <vector>
#include <cassert>
#include <algorithm>
#include <optional>
#include <numeric>

namespace
{
	using utils::Coords;
	enum SideIdx
	{
		TOP = 0,
		RIGHT,
		BOTTOM,
		LEFT,
		NUM_SIDES
	};

	auto each_direction()
	{
		return std::array<SideIdx, NUM_SIDES>{TOP, RIGHT, BOTTOM, LEFT};
	}

	SideIdx to_side_idx(int i)
	{
		if (i < 0)
		{
			const int i_pos = i * -1;
			const int cycles = 1 + i_pos / NUM_SIDES;
			const int adjustment = cycles * NUM_SIDES;
			i += adjustment;
		}
		return static_cast<SideIdx>(i % NUM_SIDES);
	}

	SideIdx opposite_side(SideIdx side)
	{
		return to_side_idx(side + 2);
	}

	using Edge = std::string;
	struct TileOption
	{
		std::array<Edge, NUM_SIDES> edges;
		std::vector<std::string> image;
		void validate() const
		{
			for (const auto& edge : edges)
			{
				assert(edge.size() == edges[0].size());
			}

			assert(!image.empty());
			for (const auto& row : image)
			{
				assert(row.size() == image.size());
				auto is_valid = [](char c) {return c == '.' || c == '#'; };
				assert(std::all_of(begin(row), end(row), is_valid));
			}

			assert(edges[0].size() == image.size() + 2);
		}
	};

	TileOption rotate_clockwise(const TileOption& original)
	{
		original.validate();
		TileOption result;

		// Start with edges.
		for (auto old_direction : each_direction())
		{
			const auto new_direction = to_side_idx(old_direction + 1);
			const bool should_reverse = (new_direction == BOTTOM || new_direction == TOP);
			const auto& old_edge = original.edges[old_direction];
			result.edges[new_direction] =
				should_reverse ? Edge(rbegin(old_edge), rend(old_edge)) : old_edge;
		}

		// Copy data across
		const auto image_size = original.image.size();
		result.image.resize(image_size, std::string(image_size, 'x'));

		for (auto old_x : utils::int_range(image_size))
		{
			const auto new_y = old_x;
			for (auto old_y : utils::int_range(image_size))
			{
				const auto new_x = image_size - old_y - 1;
				result.image[new_y][new_x] = original.image[old_y][old_x];
			}
		}
		result.validate();
		return result;
	}

	TileOption flip(TileOption tile)
	{
		std::reverse(begin(tile.image), end(tile.image));
		std::swap(tile.edges[TOP], tile.edges[BOTTOM]);
		std::reverse(begin(tile.edges[LEFT]), end(tile.edges[LEFT]));
		std::reverse(begin(tile.edges[RIGHT]), end(tile.edges[RIGHT]));
		return tile;
	}

	using TileID = int;
	TileID get_tile_id(std::string_view line)
	{
		static const std::string prefix{ "Tile " };
		static const std::string suffix{ ":" };
		assert(line.starts_with(prefix));
		assert(line.ends_with(suffix));
		line.remove_prefix(prefix.size());
		line.remove_suffix(suffix.size());
		return utils::to_value<TileID>(line);
	}

	struct Tile
	{
		std::array<TileOption, 2 * NUM_SIDES> options;
		TileID id = 0;
		void validate() const
		{
			assert(id != 0);
			for (const auto& option : options)
			{
				option.validate();
			}
		}
	};

	Tile build_tile(std::istream& input)
	{
		Tile result;

		// Get the header
		{
			std::string header;
			std::getline(input, header);
			result.id = get_tile_id(header);
		}

		// Read in the first tile option
		while (true)
		{
			std::string line;
			std::getline(input, line);

			auto& option = result.options[0];

			if (line.empty())
			{
				option.image.pop_back();
				break;
			}

			if (option.edges[TOP].empty())
			{
				option.edges[TOP] = line;
				option.edges[LEFT].reserve(line.size());
				option.edges[RIGHT].reserve(line.size());
				option.image.reserve(line.size() - 2);
			}
			else
			{
				option.image.push_back(std::string(begin(line) + 1, end(line) - 1));
			}

			option.edges[LEFT].push_back(line.front());
			option.edges[RIGHT].push_back(line.back());
			option.edges[BOTTOM] = std::move(line);
		}

		// Extrapolate the rest of the options
		for (auto i : utils::int_range<int>(1, NUM_SIDES))
		{
			result.options[i] = rotate_clockwise(result.options[i - 1]);
		}
		result.options[NUM_SIDES] = flip(result.options[NUM_SIDES - 1]);
		for (auto i : utils::int_range<int>(NUM_SIDES + 1, 2 * NUM_SIDES))
		{
			result.options[i] = rotate_clockwise(result.options[i - 1]);
		}
		return result;
	}

	std::vector<Tile> get_tiles(std::istream& input)
	{
		std::vector<Tile> result;
		while (!input.eof())
		{
			result.push_back(build_tile(input));
		}
		return result;
	}

	struct MatchData
	{
		std::size_t first_option;
		std::size_t second_option;
		SideIdx side_index;
	};

	bool do_options_match(const TileOption& first, const TileOption& second, SideIdx first_side)
	{
		const SideIdx second_side = opposite_side(first_side);
		return first.edges[first_side] == second.edges[second_side];
	}

	using MatchResult = std::optional<MatchData>;

	MatchResult get_match_data(const Tile& first, const Tile& second, SideIdx first_side, std::size_t option_idx)
	{
		if (first.id == second.id)
		{
			return std::nullopt;
		}
		assert(first_side < NUM_SIDES);
		assert(option_idx < static_cast<int64_t>(first.options.size()));
		const auto& first_option = first.options[option_idx];
		for (auto second_option : utils::int_range(second.options.size()))
		{
			if (do_options_match(first_option, second.options[second_option], first_side))
			{
				MatchData result;
				result.first_option = option_idx;
				result.second_option = second_option;
				result.side_index = first_side;
				return result;
			}
		}
		return std::nullopt;
	}

	MatchResult get_match_data(const Tile& first, const Tile& second, SideIdx first_side)
	{
		if (first.id == second.id) return std::nullopt;
		const SideIdx second_side = opposite_side(first_side);
		for (int8_t first_option : utils::int_range(static_cast<int8_t>(first.options.size())))
		{
			const TileOption& option = first.options[first_option];
			const auto result = get_match_data(first, second, first_side, first_option);
			if (result.has_value())
			{
				return result;
			}
		}
		return std::nullopt;
	}

	struct FindMatchResult
	{
		std::size_t tile_index = std::numeric_limits<std::size_t>::max();
		MatchResult result = std::nullopt;
		bool has_value() const { return result.has_value(); }
	};

	FindMatchResult find_match(const Tile& tile, std::size_t option_idx, SideIdx side, const std::vector<Tile>& tiles)
	{
		for (auto i : utils::int_range(tiles.size()))
		{
			if (tile.id == tiles[i].id) continue;
			const auto match_result = get_match_data(tile, tiles[i], side, option_idx);
			if (match_result.has_value())
			{
				return FindMatchResult{ i,match_result };
			}
		}
		return FindMatchResult{};
	}

	struct PuzzlePiece
	{
		std::size_t tile_index = std::numeric_limits<std::size_t>::max();
		std::size_t option_index = std::numeric_limits<std::size_t>::max();
		bool is_valid() const { return tile_index != std::numeric_limits<std::size_t>::max(); }
	};

	PuzzlePiece get_top_left_corner(const std::vector<Tile>& tiles)
	{
		for (auto t : utils::int_range(tiles.size()))
		{
			const Tile& tile = tiles[t];
			for (auto o : utils::int_range(tile.options.size()))
			{
				const auto directions = each_direction();
				std::array<MatchResult, NUM_SIDES> dir_results;
				assert(directions.size() == dir_results.size());
				std::transform(begin(directions), end(directions), begin(dir_results),
					[&tile, o,&tiles](SideIdx side)
				{
					return find_match(tile, o, side, tiles).result;
				});
				if (dir_results[RIGHT].has_value() &&
					dir_results[BOTTOM].has_value() &&
					!dir_results[LEFT].has_value() &&
					!dir_results[TOP].has_value())
				{
					PuzzlePiece result;
					result.tile_index = t;
					result.option_index = o;
					return result;
				}
			}
		}
		assert(false);
		return PuzzlePiece{};
	}

	std::vector<std::vector<PuzzlePiece>> put_image_together(const std::vector<Tile>& tiles)
	{
		const auto expected_size = utils::isqrt(tiles.size());
		assert(expected_size * expected_size == tiles.size());
		std::vector<std::vector<PuzzlePiece>> result;
		result.reserve(expected_size);

		while (true) // Per row
		{
			std::vector<PuzzlePiece> current_row;
			current_row.reserve(expected_size);
			if (result.empty())
			{
				const auto start = get_top_left_corner(tiles);
				assert(start.is_valid());
				current_row.push_back(start);
			}
			else
			{
				const auto& one_up = result.back().front();
				assert(one_up.is_valid());
				const auto start_match = find_match(tiles[one_up.tile_index], one_up.option_index, BOTTOM, tiles);
				if (start_match.has_value())
				{
					assert(start_match.result.has_value());
					assert(start_match.result->first_option == one_up.option_index);
					assert(start_match.result->side_index == BOTTOM);
					PuzzlePiece start;
					start.tile_index = start_match.tile_index;
					start.option_index = start_match.result->second_option;
					current_row.push_back(start);
				}
				else
				{
					break;
				}
				
			}

			while (true) // Per column
			{
				assert(!current_row.empty());
				const auto previous = current_row.back();
				assert(previous.is_valid());
				const auto next_match = find_match(tiles[previous.tile_index], previous.option_index, RIGHT, tiles);
				if (next_match.has_value())
				{
					assert(next_match.result.has_value());
					assert(next_match.result->side_index == RIGHT);
					assert(next_match.result->first_option == previous.option_index);
					PuzzlePiece next;
					next.tile_index = next_match.tile_index;
					next.option_index = next_match.result->second_option;
					current_row.push_back(next);
				}
				else
				{
					break;
				}
			}
			assert(current_row.size() == expected_size);
			result.push_back(std::move(current_row));
		}
		assert(result.size() == expected_size);
		return result;
	}

	using Image = std::vector<std::string>;

	Image construct_image(std::vector<Tile> tiles)
	{
		assert(!tiles.empty());
		const auto solution = put_image_together(tiles);
		const auto num_tiles_per_side = utils::isqrt(tiles.size());
		assert(num_tiles_per_side * num_tiles_per_side == tiles.size());
		const auto num_rows = num_tiles_per_side * tiles.front().options.front().image.size();

		Image result;
		result.reserve(num_rows);
		for (const auto& row : solution)
		{
			assert(!row.empty());
			std::vector<std::ostringstream> section;
			section.resize(tiles.front().options.front().image.size());

			for (const auto& tile_option : row)
			{
				assert(tile_option.tile_index < tiles.size());
				Tile& tile = tiles[tile_option.tile_index];
				assert(tile_option.option_index < tile.options.size());
				TileOption& option = tile.options[tile_option.option_index];
				assert(option.image.size() == section.size());
				for (auto i : utils::int_range(option.image.size()))
				{
					section[i] << std::move(option.image[i]);
				}
			}

			std::transform(begin(section), end(section), std::back_inserter(result),
				[num_rows](std::ostringstream& oss)
			{
				std::string result = oss.str();
				assert(result.size() == num_rows);
				return result;
			});
		}
		assert(result.size() == num_rows);
		return result;
	}

	int count_matches(const Tile& tile, const std::vector<Tile> tiles)
	{
		auto has_any_match = [&tile](const Tile& other)
		{
			const auto dirs = each_direction();
			return std::any_of(begin(dirs), end(dirs),
				[&tile, &other](SideIdx si)
			{
				const auto result = get_match_data(tile, other, si);
				return result.has_value();
			});
		};
		return std::count_if(begin(tiles), end(tiles), has_any_match);
	}

	std::vector<TileID> get_corners(const std::vector<Tile>& tiles)
	{
		std::vector<TileID> result;
		result.reserve(4);
		utils::transform_if(begin(tiles), end(tiles), std::back_inserter(result),
			[](const Tile& t) {return t.id; },
			[&tiles](const Tile& t) {return count_matches(t, tiles) == 2; });
		assert(result.size() == 4);
		return result;
	}

	std::vector<TileID> get_corners_from_input(std::istream& input)
	{
		const auto tiles = get_tiles(input);
		return get_corners(tiles);
	}

	std::string get_sea_monster_image()
	{
		return std::string(
			"                  # \n"
			"#    ##    ##    ###\n"
			" #  #  #  #  #  #   ");
	}

	using Signature = std::vector<Coords>;
	Signature get_image_signature(std::string str)
	{
		Signature result;
		std::istringstream input{ std::move(str) };
		int y = 0;
		for (auto it = utils::istream_line_iterator(input); it != utils::istream_line_iterator(); ++it, ++y)
		{
			const std::string line = *it;
			for (auto i : utils::int_range(line.size()))
			{
				const char loc = line[i];
				if (loc == '#')
				{
					result.push_back(Coords(i, y));
				}
			}
		}
		return result;
	}
	
	std::array<Signature,8> get_all_signatures(const Signature& base)
	{
		std::array<Signature, 8> result;
		result[0] = base;
		std::transform(begin(base), end(base), std::back_inserter(result[1]),
			[](Coords in) {return Coords(in.x, -in.y); });
		std::transform(begin(base), end(base), std::back_inserter(result[2]),
			[](Coords in) {return Coords(-in.x, in.y); });
		std::transform(begin(base), end(base), std::back_inserter(result[3]),
			[](Coords in) {return Coords(-in.x, -in.y); });
		std::transform(begin(base), end(base), std::back_inserter(result[4]),
			[](Coords in) {return Coords(in.y, in.x); });
		std::transform(begin(base), end(base), std::back_inserter(result[5]),
			[](Coords in) {return Coords(in.y, -in.x); });
		std::transform(begin(base), end(base), std::back_inserter(result[6]),
			[](Coords in) {return Coords(-in.y, in.x); });
		std::transform(begin(base), end(base), std::back_inserter(result[7]),
			[](Coords in) {return Coords(-in.y, -in.x); });
		return result;
	}

	bool spot_contains_signature(const Image& image, const Signature& sig, Coords loc)
	{
		auto is_part_of_signature = [&](Coords offset)
		{
			const Coords final_loc = loc + offset;
			if (!utils::in_range<int>(final_loc.y, 0, static_cast<int>(image.size() - 1)))
			{
				return false;
			}
			std::string_view line = image[final_loc.y];
			if (!utils::in_range<int>(final_loc.x, 0, static_cast<int>(line.size() - 1)))
			{
				return false;
			}
			return line[final_loc.x] == '#';
		};
		return std::all_of(begin(sig), end(sig), is_part_of_signature);
	}

	int count_signatures(const Image& image, const Signature& signature, Coords starting_point)
	{
		int x = starting_point.x;
		int result = 0;
		for (int y = starting_point.y; y < static_cast<int>(image.size()); ++y)
		{
			std::string_view line = image[y];
			for (; x < static_cast<int>(line.size()); ++x)
			{
				if (spot_contains_signature(image, signature, Coords{ x,y }))
				{
					++result;
				}
			}
			x = 0;
		}
		return result;
	}

	auto spot_contains_signature(const Image& image, const std::array<Signature, 8>& sigs, Coords loc)
	{
		return std::find_if(begin(sigs), end(sigs),
			[&](const Signature& s)
		{
			return spot_contains_signature(image, s, loc);
		});
	}

	int count_signatures(const Image& image, const std::array<Signature, 8>& sigs)
	{
		for (auto y : utils::int_range(static_cast<int>(image.size())))
		{
			for (auto x : utils::int_range(static_cast<int>(image[y].size())))
			{
				const Coords loc{ x, y };
				auto has_sig_result = spot_contains_signature(image, sigs, loc);
				if (has_sig_result != end(sigs))
				{
					return count_signatures(image, *has_sig_result, loc);
				}
			}
		}
		return 0;
	}

	int64_t solve_p1(std::istream& input)
	{
		const auto corners = get_corners_from_input(input);
		return std::reduce(begin(corners), end(corners), int64_t{ 1 }, std::multiplies<int64_t>{});
	}

	int64_t solve_p2(std::istream& input)
	{
		auto tiles = get_tiles(input);
		const Image image = construct_image(std::move(tiles));
		const auto sea_monster = get_sea_monster_image();
		const auto sea_monster_sig = get_image_signature(sea_monster);
		const auto num_sea_monsters = count_signatures(image, get_all_signatures(sea_monster_sig));

		const auto num_tiles = std::transform_reduce(begin(image), end(image), 0, std::plus<int>{},
			[](std::string_view line)
		{
			return std::count(begin(line), end(line), '#');
		});

		return num_tiles - (num_sea_monsters * sea_monster_sig.size());
	}
}

ResultType day_twenty_testcase_a()
{
	auto input = utils::open_testcase_input(20, 'a');
	return solve_p1(input);
}

ResultType advent_twenty_p1()
{
	auto input = utils::open_puzzle_input(20);
	return solve_p1(input);
}

ResultType day_twenty_testcase_b()
{
	auto input = utils::open_testcase_input(20, 'a');
	return solve_p2(input);
}

ResultType advent_twenty_p2()
{
	auto input = utils::open_puzzle_input(20);
	return solve_p2(input);
}