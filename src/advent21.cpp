#include "../advent/advent21.h"
#include "../utils/advent_utils.h"

#include "../utils/split_string.h"
#include "../utils/trim_string.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/int_range.h"
#include "../utils/sorted_vector.h"

#include <cassert>
#include <compare>
#include <string>
#include <unordered_map>
#include <numeric>
#include <sstream>

namespace
{
	using utils::sorted_vector;
	using utils::split_string;
	using utils::istream_line_iterator;

	struct Allergen
	{
		std::string name;
		sorted_vector<std::string> possible_ingredients;
		bool operator<(const Allergen& other) const noexcept
		{
			return name < other.name;
		}
		bool operator==(const Allergen& other) const noexcept
		{
			return name == other.name;
		}
	};

	using IngredientsAndAppearences = std::unordered_map<std::string, int>;
	using AllergenList = sorted_vector<Allergen>;

	// Returns list of ingredients (first) and list of allergens (second)
	std::pair<std::vector<std::string_view>, std::vector<std::string_view>> parse_line(std::string_view line)
	{
		auto [ingredients_view, allergens_view] = [&]()
		{
			auto result = split_string(line, '(');
			assert(result.size() == 2);
			auto& ingredients = result[0];
			auto& allergens = result[1];
			assert(ingredients.back() == ' ');
			ingredients.remove_suffix(1);
			assert(allergens.back() == ')');
			allergens.remove_suffix(1);
			assert(allergens.starts_with("contains "));
			allergens.remove_prefix(9);
			return std::make_pair(ingredients,allergens);
		}();

		auto ingredients = split_string(ingredients_view, ' ');
		auto allergens = split_string(allergens_view, ',');
		std::transform(begin(allergens), end(allergens), begin(allergens),
			[](std::string_view allergen)
		{
			return utils::trim_left(allergen);
		});
		return std::make_pair(ingredients, allergens);
	}

	void combine_ingredient(AllergenList& running_result, Allergen new_allergen)
	{
		const auto previous = running_result.find(new_allergen);
		if (previous != end(running_result))
		{
			sorted_vector<std::string> new_possible_ingredients;
			std::set_intersection(
				begin(new_allergen.possible_ingredients), end(new_allergen.possible_ingredients),
				begin(previous->possible_ingredients), end(previous->possible_ingredients),
				std::back_inserter(new_possible_ingredients));
			previous->possible_ingredients = std::move(new_possible_ingredients);
		}
		else
		{
			running_result.insert(new_allergen);
		}
	}

	void combine_ingredients(
		AllergenList& running_result,
		const std::vector<std::string_view>& ingredients,
		const std::vector<std::string_view>& allergens)
	{
		const sorted_vector<std::string> ingredient_set = [&ingredients]()
		{
			sorted_vector<std::string> result;
			std::transform(begin(ingredients), end(ingredients), std::back_inserter(result),
				[](std::string_view sv) {return std::string{ sv }; });
			return result;
		}();

		for (auto allergen : allergens)
		{
			combine_ingredient(running_result, Allergen{ std::string{allergen},ingredient_set });
		}
	}


	struct IngredientsAndAllergens
	{
		IngredientsAndAppearences ingredients;
		AllergenList allergens;
	};

	AllergenList reduce_possibilities(AllergenList allergens, std::vector<bool>& checked_allergens, std::size_t index)
	{
		assert(allergens[index].possible_ingredients.size() == 1);
		assert(index < allergens.size());
		assert(checked_allergens.size() == allergens.size());
		if (checked_allergens[index])
		{
			return allergens;
		}

		checked_allergens[index] = true;

		const std::string& ingredient_name = allergens[index].possible_ingredients[0];
		for (auto i : utils::int_range(allergens.size()))
		{
			if (i == index) continue;
			auto& ingredients = allergens[i].possible_ingredients;
			if (ingredients.size() == 1) continue;
			ingredients.erase(ingredient_name);
			if (ingredients.size() == 1)
			{
				allergens = reduce_possibilities(std::move(allergens), checked_allergens, i);
			}
		}
		return allergens;
	}

	// Reduce possible allergen lists based on which ones only
	// have a single possibility. Remove that possibility from other lists.
	AllergenList reduce_possibilities(AllergenList allergens)
	{
		std::vector<bool> checked_allergens(allergens.size(), false);
		for (auto i : utils::int_range(allergens.size()))
		{
			if (allergens[i].possible_ingredients.size() == 1)
			{
				allergens = reduce_possibilities(std::move(allergens), checked_allergens, i);
			}
		}
		for (const auto& a : allergens)
		{
			assert(a.possible_ingredients.size() == 1);
		}
		return allergens;
	}

	IngredientsAndAllergens extract_ingredients_and_allergens(std::istream& input)
	{
		IngredientsAndAppearences ingredients;
		AllergenList allergens;
		std::for_each(istream_line_iterator{ input }, istream_line_iterator{},
			[&allergens,&ingredients](std::string_view line)
		{
			const auto [ingredient_list, allergen_list] = parse_line(line);
			combine_ingredients(allergens, ingredient_list, allergen_list);
			for (auto ingredient : ingredient_list)
			{
				ingredients[std::string{ ingredient }] += 1;
			}
		});
		allergens = reduce_possibilities(std::move(allergens));
		return IngredientsAndAllergens{ std::move(ingredients), std::move(allergens) };
	}

	IngredientsAndAppearences get_safe_ingredients(IngredientsAndAllergens input_data)
	{
		IngredientsAndAppearences& ingredients = input_data.ingredients;
		for (const Allergen& allergen : input_data.allergens)
		{
			assert(allergen.possible_ingredients.size() == 1);
			ingredients.erase(allergen.possible_ingredients[0]);
		}
		return std::move(ingredients);
	}

	AllergenList extract_allergens(std::istream& input)
	{
		return extract_ingredients_and_allergens(input).allergens;
	}

	int solve_p1(std::istream& input)
	{
		IngredientsAndAllergens data = extract_ingredients_and_allergens(input);
		const IngredientsAndAppearences safe_ingredients = get_safe_ingredients(std::move(data));
		auto get_num_appearances = [](const IngredientsAndAppearences::value_type& ingredient)
		{
			return ingredient.second;
		};
		return std::transform_reduce(begin(safe_ingredients), end(safe_ingredients), 0,
			std::plus<int>{}, get_num_appearances);
	}

	std::string solve_p2(std::istream& input)
	{
		auto allergens = extract_allergens(input);
		std::ostringstream output;
		std::transform(begin(allergens), end(allergens), std::ostream_iterator<std::string>(output, ","),
			[](const Allergen& a)
		{
			assert(a.possible_ingredients.size() == 1);
			return a.possible_ingredients[0];
		});
		std::string result = output.str();
		assert(result.back() == ',');
		result.pop_back();
		return result;
	}

	std::istringstream get_testcase_a()
	{
		return std::istringstream{
			"mxmxvkd kfcds sqjhc nhms (contains dairy, fish)\n"
			"trh fvjkl sbzzf mxmxvkd (contains dairy)\n"
			"sqjhc fvjkl (contains soy)\n"
			"sqjhc mxmxvkd sbzzf (contains fish)"
		};
	}
}

ResultType day_twentyone_testcase_a()
{
	auto input = get_testcase_a();
	return solve_p1(input);
}

ResultType advent_twentyone_p1()
{
	auto input = utils::open_puzzle_input(21);
	return solve_p1(input);
}
ResultType day_twentyone_testcase_b()
{
	auto input = get_testcase_a();
	return solve_p2(input);
}


ResultType advent_twentyone_p2()
{
	auto input = utils::open_puzzle_input(21);
	return solve_p2(input);
}
