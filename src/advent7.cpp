#include "../advent/advent7.h"
#include "../utils/advent_utils.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/binary_find.h"

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <numeric>

namespace
{
	using Bag = std::ptrdiff_t;
	using namespace utils;

	Bag to_bag(const std::string& str)
	{
		static std::vector<std::string> bag_list;
		const auto loc = std::find(begin(bag_list), end(bag_list), str);
		if (loc != end(bag_list))
		{
			return std::distance(begin(bag_list), loc);
		}
		else
		{
			const int result = static_cast<Bag>(bag_list.size());
			bag_list.push_back(str);
			return result;
		}
	}
	
	struct PermittedBag
	{
		int amount = 0;
		Bag bag = -1;
	};
	enum class CanHoldReference : char
	{
		no,
		yes,
		unknown
	};
	struct Rule
	{
		std::vector<PermittedBag> permitted_bags;
		Bag bag;
		mutable CanHoldReference can_hold = CanHoldReference::unknown;
		mutable int extra_bags_required = -1;
		bool operator<(const Rule& other) const { return bag < other.bag; }
	};

	using Ruleset = std::vector<Rule>;

	Bag extract_bag(std::istream& input, const std::string& expected_end)
	{
		std::ostringstream bag_id;
		while (!input.eof())
		{
			std::string next_word;
			input >> next_word;
			if (next_word == "bag" || next_word == "bags" || next_word.empty())
			{
				assert(next_word == expected_end);
				return to_bag(bag_id.str());
			}
			else
			{
				bag_id << next_word;
			}
		}
		assert(false);
		return -1;
	}

	Bag extract_bag(std::string input, const std::string& expected_end)
	{
		std::istringstream iss{ std::string{input} };
		return extract_bag(iss, expected_end);
	}

	PermittedBag extract_permitted_bags(std::string str)
	{
		PermittedBag result;
		if (str == " no other bags")
		{
			return result;
		}
		std::istringstream input{ std::move(str) };
		input >> result.amount;
		result.bag = extract_bag(input,result.amount == 1 ? "bag" : "bags");
		return result;
	}

	Rule parse_rule(std::string rulestring)
	{
		assert(rulestring.back() == '.');
		rulestring.pop_back();
		std::istringstream input{ std::move(rulestring) };

		Rule result;
		result.bag = extract_bag(input, "bags");
		std::string dummy;
		input >> dummy;
		assert(dummy == "contain");
		
		std::transform(istream_line_iterator(input, ','), istream_line_iterator(),
			std::back_inserter(result.permitted_bags), extract_permitted_bags);

		return result;
	}

	Ruleset parse_rules(std::istream& input)
	{
		Ruleset result;
		std::transform(istream_line_iterator(input), istream_line_iterator(),
			std::back_inserter(result), parse_rule);
		std::sort(begin(result), end(result));
		return result;
	}

	auto get_rule_iterator(const Ruleset& rules, Bag target)
	{
		Rule reference;
		reference.bag = target;
		return binary_find(begin(rules), end(rules), reference);
	}

	bool can_bag_contain(Bag container, const Ruleset& rules, Bag target)
	{
		if (container == target) return false;

		const auto rule_it = get_rule_iterator(rules, container);
		if (rule_it == end(rules)) return false;
		switch (rule_it->can_hold)
		{
		case CanHoldReference::yes:
			return true;
		case CanHoldReference::no:
			return false;
		default:
			break;
		}

		const auto& allowed_bags = rule_it->permitted_bags;
		if (allowed_bags.size() == 0u)
		{
			rule_it->can_hold = CanHoldReference::no;
			return false;
		}

		const auto find_result = std::find_if(begin(allowed_bags), end(allowed_bags),
			[target](const PermittedBag& pb) {return pb.bag == target; });

		if (find_result != end(allowed_bags))
		{
			rule_it->can_hold = CanHoldReference::yes;
			return true;
		}

		const bool result = std::any_of(begin(allowed_bags), end(allowed_bags),
			[target, &rules](const PermittedBag& pb) {return can_bag_contain(pb.bag, rules, target); });

		rule_it->can_hold = result ? CanHoldReference::yes : CanHoldReference::no;
		return result;
	}

	int solve_p1_generic(const Ruleset& rules, Bag bag)
	{
		return std::count_if(begin(rules), end(rules),
			[bag,&rules](const Rule& r)
		{
			return can_bag_contain(r.bag, rules, bag);
		});
	}

	int solve_p1(std::istream& input)
	{
		const Ruleset rules = parse_rules(input);
		return solve_p1_generic(rules, extract_bag("shiny gold bag", "bag"));
	}

	int solve_p2_generic(const Ruleset& rules, Bag bag)
	{
		const auto rule_it = get_rule_iterator(rules, bag);
		if (rule_it == end(rules)) return 0;

		if (rule_it->extra_bags_required != -1)
		{
			return rule_it->extra_bags_required;
		}

		const auto& contained_bags = rule_it->permitted_bags;
		auto get_all_bags = [&rules](int prev, const PermittedBag& pb)
		{
			const int per_bag = solve_p2_generic(rules, pb.bag);
			return prev + pb.amount * (1 + per_bag);
		};

		const int result = std::accumulate(begin(contained_bags), end(contained_bags),
			0, get_all_bags);
		rule_it->extra_bags_required = result;
		return result;
	}

	int solve_p2(std::istream& input)
	{
		const Ruleset rules = parse_rules(input);
		return solve_p2_generic(rules, extract_bag("shiny gold bag", "bag"));
	}
}

ResultType day_seven_testcase_a()
{
	std::ifstream input = open_testcase_input(7, 'a');
	return solve_p1(input);
}

ResultType advent_seven_p1()
{
	auto input = open_puzzle_input(7);
	return solve_p1(input);
}

ResultType day_seven_testcase_b()
{
	std::ifstream input = open_testcase_input(7, 'a');
	return solve_p2(input);
}

ResultType day_seven_testcase_c()
{
	std::ifstream input = open_testcase_input(7, 'c');
	return solve_p2(input);
}

ResultType advent_seven_p2()
{
	auto input = open_puzzle_input(7);
	return solve_p2(input);
}
