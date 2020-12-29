#include "../advent/advent22.h"
#include "../utils/advent_utils.h"
#include "../utils/int_range.h"
#include "../utils/to_value.h"

#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <set>
#include <optional>

namespace
{
	using Card = uint8_t;
	using Deck = std::vector<Card>;
	using GameState = std::pair<Deck, Deck>;
	
	Deck front_to_back(Deck deck)
	{
		std::rotate(begin(deck), begin(deck) + 1, end(deck));
		return deck;
	}

	Deck resolve_winners_deck(Deck before, Card new_card)
	{
		before = front_to_back(std::move(before));
		before.push_back(new_card);
		return before;
	}

	Deck resolve_losers_deck(Deck before)
	{
		before = front_to_back(std::move(before));
		before.pop_back();
		return before;
	}

	void update_decks(Deck& winner, Deck& loser)
	{
		Card moved_card = loser.front();
		winner = resolve_winners_deck(std::move(winner), moved_card);
		loser = resolve_losers_deck(std::move(loser));
	}

	GameState play_turn(GameState game_state)
	{
		Deck& a = game_state.first;
		Deck& b = game_state.second;
		assert(!a.empty());
		assert(!b.empty());
		assert(a.front() != b.front());
		
		const bool a_wins = a.front() > b.front();
		Deck& winner = a_wins ? a : b;
		Deck& loser = !a_wins ? a : b;
		update_decks(winner, loser);
		return game_state;
	}

	Deck get_winning_deck(GameState state)
	{
		while (!state.first.empty() && !state.second.empty())
		{
			state = play_turn(std::move(state));
		}
		return std::move(state.first.empty() ? state.second : state.first);
	}

	struct GameResult
	{
		Deck winning_deck;
		bool player_one_wins;
	};

	GameState play_turn_recursive(GameState state);

	GameResult get_winning_deck_recursive(GameState state)
	{
		// Check for loop win.
		std::set<GameState> previous_states;
		while (!state.first.empty() && !state.second.empty())
		{
			{
				if (previous_states.contains(state))
				{
					return GameResult{ std::move(state.first) , true };
				}
				else
				{
					previous_states.insert(state);
				}

			}

			state = play_turn_recursive(std::move(state));
		}
		if (state.first.empty())
		{
			return GameResult{ std::move(state.second),false };
		}
		else
		{
			assert(state.second.empty());
			return GameResult{ std::move(state.first),true };
		}
	}

	Deck get_sub_deck(const Deck& d,std::size_t num_cards)
	{
		const Card card = d.front();
		Deck result;
		if (d.size() > card)
		{
			result.reserve(num_cards);
			result.insert(end(result),begin(d) + 1, begin(d) + card + 1);
		}
		return result;
	}

	GameState play_turn_recursive(GameState state)
	{
		Deck& p1 = state.first;
		Deck& p2 = state.second;
		
		const std::size_t num_cards = p1.size() + p2.size();
		Deck subdeck_p1 = get_sub_deck(p1,num_cards);
		Deck subdeck_p2 = get_sub_deck(p2,num_cards);

		if (subdeck_p1.empty() || subdeck_p2.empty())
		{
			return play_turn(std::move(state));
		}

		const auto result = get_winning_deck_recursive(GameState(std::move(subdeck_p1), std::move(subdeck_p2)));
		Deck& winner = result.player_one_wins ? p1 : p2;
		Deck& loser = !result.player_one_wins ? p1 : p2;
		update_decks(winner, loser);
		return state;
	}

	int64_t score_deck(const Deck& deck)
	{
		return std::inner_product(rbegin(deck), rend(deck), begin(utils::int_range<int>(1,deck.size()+1)), int64_t{ 0 });
	}

	Deck extract_deck(std::istream& input)
	{
		std::string line;
		std::getline(input, line);
		assert(line == "Player 1:" || line == "Player 2:");

		Deck result;
		while(!input.eof())
		{
			std::getline(input, line);
			if (line.empty()) break;
			result.push_back(utils::to_value<Card>(line));
		}
		return result;
	}

	GameState extract_game_state(std::istream& input)
	{
		GameState result;
		result.first = extract_deck(input);
		result.second = extract_deck(input);
		const auto num_cards = result.first.size() + result.second.size();
		result.first.reserve(num_cards);
		result.second.reserve(num_cards);
		return result;
	}

	int64_t solve_p1(std::istream& input)
	{
		GameState initial_state = extract_game_state(input);
		const Deck winner = get_winning_deck(std::move(initial_state));
		return score_deck(winner);
	}

	int64_t solve_p2(std::istream& input)
	{
		GameState initial_state = extract_game_state(input);
		const auto result = get_winning_deck_recursive(std::move(initial_state));
		return score_deck(result.winning_deck);
	}
}

ResultType day_twentytwo_testcase_a()
{
	auto input = utils::open_testcase_input(22, 'a');
	return solve_p1(input);
}

ResultType advent_twentytwo_p1()
{
	auto input = utils::open_puzzle_input(22);
	return solve_p1(input);
}

ResultType day_twentytwo_testcase_b()
{
	auto input = utils::open_testcase_input(22, 'a');
	return solve_p2(input);
}

ResultType advent_twentytwo_p2()
{
	auto input = utils::open_puzzle_input(22);
	return solve_p2(input);
}
