#include "../advent/advent23.h"

#include "../utils/int_range.h"
#include "../utils/in_range.h"

#include <algorithm>
#include <string>
#include <cassert>
#include <numeric>
#include <array>

namespace
{
	constexpr std::size_t NUM_PICKED_UP = 3;
	using Cup = std::size_t;
	class State
	{
		std::vector<Cup> m_next_cup;
		Cup try_get_destination_cup(const std::array<Cup,NUM_PICKED_UP>& picked_up_cups, Cup trial_destination) const
		{
			if (std::find(begin(picked_up_cups), end(picked_up_cups), trial_destination) != end(picked_up_cups))
			{
				return try_get_destination_cup(picked_up_cups, trial_destination - 1);
			}
			else if (trial_destination >= m_next_cup.size())
			{
				return try_get_destination_cup(picked_up_cups,m_next_cup.size() - 1);
			}
			return trial_destination;
		}
	public:
		State(std::string_view initial_data, std::size_t total_size)
		{
			auto get_cup = [&initial_data, total_size](std::size_t index)
			{
				index = index % total_size;
				return (index < initial_data.size() ? initial_data[index] - '0' : index+1) % total_size;
			};

			m_next_cup.resize(total_size);
			for (auto i : utils::int_range(total_size))
			{
				const Cup current = get_cup(i);
				const Cup next = get_cup(i + 1);
				m_next_cup[current] = next;
			}
		}

		State(std::string_view initial_data) : State(initial_data, initial_data.size()) {}

		Cup get_next_cup_after(Cup cup) const noexcept
		{
			assert(cup <= m_next_cup.size());
			if (cup == m_next_cup.size()) cup = 0;
			const Cup result = m_next_cup[cup];
			return result != 0 ? result : m_next_cup.size();
		}

		// Returns next current cup
		Cup take_turn(Cup current_cup)
		{
			const std::array<Cup, NUM_PICKED_UP> picked_up_cups = [this](Cup current)
			{
				std::array<Cup, NUM_PICKED_UP> result;
				for (auto& c : result)
				{
					current = m_next_cup[current];
					c = current;
				}
				return result;
			}(current_cup);

			const Cup destination = try_get_destination_cup(picked_up_cups, current_cup - 1);

			const Cup after_destination = m_next_cup[destination];
			m_next_cup[destination] = picked_up_cups.front();
			

			const Cup next_current = m_next_cup[picked_up_cups.back()];
			m_next_cup[picked_up_cups.back()] = after_destination;
			m_next_cup[current_cup] = next_current;

//			for (auto i : utils::int_range(m_next_cup.size()))
//			{
//				assert(std::count(begin(m_next_cup), end(m_next_cup), i) == 1);
//			}

			return next_current;
		}
	};

	State play_game(State state, std::size_t num_moves, Cup start)
	{
		Cup current_cup = start;

		for (auto i : utils::int_range(num_moves))
		{
			current_cup = state.take_turn(current_cup);
		}
		return state;
	}

	int64_t get_result_p1(const State& state, Cup anchor)
	{
		int64_t result = 0;
		Cup current = state.get_next_cup_after(anchor);
		while (current != anchor)
		{
			result *= 10;
			result += current;
			current = state.get_next_cup_after(current);
		}
		return result;
	}

	int64_t solve_p1(const std::string& input, std::size_t num_moves)
	{
		State state{ input };
		state = play_game(std::move(state), num_moves,(input[0] - '0') % input.size());
		return get_result_p1(state,1);
	}

	int64_t get_result_p2(const State& state, Cup anchor)
	{
		const Cup cup_a = state.get_next_cup_after(anchor);
		const Cup cup_b = state.get_next_cup_after(cup_a);
		return static_cast<int64_t>(cup_a) * static_cast<int64_t>(cup_b);
	}

	int64_t solve_p2(const std::string& input)
	{
		State state{ input,1'000'000 };
		state = play_game(std::move(state), 10'000'000,input[0] - '0');
		return get_result_p2(state,1);
	}

	std::string get_p1_testcase()
	{
		return "389125467";
	}

	std::string get_puzzle_input()
	{
		return "942387615";
	}
}

ResultType day_twentythree_testcase_a()
{
	return solve_p1(get_p1_testcase(), 10);
}

ResultType day_twentythree_testcase_b()
{
	return solve_p1(get_p1_testcase(), 100);
}

ResultType day_twentythree_testcase_c()
{
	return solve_p2(get_p1_testcase());
}

ResultType advent_twentythree_p1()
{
	return solve_p1(get_puzzle_input(), 100);
}

ResultType advent_twentythree_p2()
{
	return solve_p2(get_puzzle_input());
}
