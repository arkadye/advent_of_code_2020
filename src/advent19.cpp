#include "../advent/advent19.h"
#include "../utils/advent_utils.h"
#include "../utils/trim_string.h"
#include "../utils/to_value.h"
#include "../utils/split_string.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/int_range.h"

#include <vector>
#include <variant>
#include <algorithm>
#include <unordered_map>
#include <optional>

#ifndef NDEBUG
#define DAY19DEBUG 0
#else
#define DAY19DEBUG 0
#endif

#if DAY19DEBUG
#include <iostream>
#endif

namespace
{
	using RuleID = std::size_t;
	using Sequence = std::vector<RuleID>;
	using PatternRule = std::string;
	using SequenceOption = std::variant<PatternRule, Sequence>;
	using CallingRule = std::vector<SequenceOption>;
	using Rule = std::variant<CallingRule, PatternRule>;
	using RuleSet = std::vector<Rule>;

	template <typename T>
	bool is_rule_type(const Rule& rule)
	{
		return std::holds_alternative<T>(rule);
	}

	bool is_calling_rule(const Rule& rule)
	{
		return is_rule_type<CallingRule>(rule);
	}

	bool is_pattern_rule(const Rule& rule)
	{
		return is_rule_type<PatternRule>(rule);
	}

	template <typename T>
	const T* get_rule_as(const Rule& rule)
	{
		return std::get_if<T>(&rule);
	}

	auto get_as_pattern_rule(const Rule& rule)
	{
		return get_rule_as<PatternRule>(rule);
	}

	auto get_as_calling_rule(const Rule& rule)
	{
		return get_rule_as<CallingRule>(rule);
	}

	PatternRule parse_as_pattern_rule(std::string_view rule)
	{
		assert(!rule.empty());
		assert(rule.front() == '"');
		assert(rule.back() == '"');
		rule.remove_prefix(1);
		rule.remove_suffix(1);
		return std::string{ rule };
	}

	RuleID parse_as_rule_id(std::string_view rule)
	{
		rule = utils::trim_string(rule);
		assert(!rule.empty());
		return utils::to_value<RuleID>(rule);
	}

	Sequence parse_as_sequence(std::string_view rule)
	{
		rule = utils::trim_string(rule);
		assert(!rule.empty());
		const auto split = utils::split_string(rule,' ');
		Sequence result;
		result.reserve(split.size());
		std::transform(begin(split), end(split), std::back_inserter(result), parse_as_rule_id);
		return result;
	}

	CallingRule parse_as_calling_rule(std::string_view rule)
	{
		assert(!rule.empty());
		const auto sequences = utils::split_string(rule, '|');
		CallingRule result;
		result.reserve(sequences.size());
		std::transform(begin(sequences), end(sequences), std::back_inserter(result), parse_as_sequence);
		return result;
	}

	Rule parse_rule(std::string_view rule)
	{
		rule = utils::trim_string(rule);
		assert(!rule.empty());
		if (rule.front() == '"')
		{
			return parse_as_pattern_rule(rule);
		}
		else
		{
			return parse_as_calling_rule(rule);
		}
	}

	std::pair<RuleID, Rule> parse_line(std::string_view line)
	{
		assert(std::count(begin(line), end(line), ':') == 1);
		const auto parts = utils::split_string(line, ':');
		assert(parts.size() == 2);
		const RuleID id = parse_as_rule_id(parts[0]);
		Rule rule = parse_rule(parts[1]);
		return std::make_pair(id, std::move(rule));
	}

	void verify_ruleset(const RuleSet& rules)
	{
		for (const Rule& rule : rules)
		{
			if (auto pCallingRule = get_as_calling_rule(rule))
			{
				for (const SequenceOption& sequence_option : *pCallingRule)
				{
					if (auto pSequence = std::get_if<Sequence>(&sequence_option))
					{
						for (RuleID id : *pSequence)
						{
							assert(id < rules.size());
						}
					}
				}
			}
		}
	}

	RuleSet extract_ruleset(std::istream& input)
	{
		std::string line;
		RuleSet result;
		do
		{
			std::getline(input, line);
			if (!line.empty())
			{
				auto [id, rule] = parse_line(line);
				if (id >= result.size())
				{
					result.resize(id + 1);
				}
				result[id] = std::move(rule);
			}
		} while (!line.empty());
		verify_ruleset(result);
		return result;
	}

	// If there are multiple good options to parse, this returns all of them.
	// If the parse is unsuccessful, empty.
	using RuleCheckResult = std::vector<std::string_view>;

	RuleCheckResult check_rule_by_id(const RuleSet& rules, RuleID id, std::string_view input);

	RuleCheckResult check_rule_pattern(const PatternRule& rule, std::string_view input)
	{
		if (input.empty())
		{
			return RuleCheckResult{};
		}
		else if (input.starts_with(rule))
		{
			input.remove_prefix(rule.size());
			return RuleCheckResult{ input };
		}
		else
		{
			return RuleCheckResult{};
		}
	}

	RuleCheckResult check_rule_sequence(
		const RuleSet& rules,
		Sequence::const_iterator first,
		Sequence::const_iterator last,
		std::string_view input)
	{
		if (first == last) return RuleCheckResult{ input };
		RuleCheckResult result;
		const RuleID id = *first;
		const RuleCheckResult id_outcome = check_rule_by_id(rules, id, input);
		for (const std::string_view& remaining : id_outcome)
		{
			const RuleCheckResult parse_outcome = check_rule_sequence(rules, first + 1, last, remaining);
			std::copy(begin(parse_outcome), end(parse_outcome), std::back_inserter(result));
		}
		return result;
	}

	RuleCheckResult check_rule_sequence(const RuleSet& rules, const Sequence& sequence, std::string_view input)
	{
		return check_rule_sequence(rules, begin(sequence), end(sequence), input);
	}

	RuleCheckResult check_rule_sequence_option(const RuleSet& rules, const SequenceOption& sequence_option, std::string_view input)
	{
		if (auto pPattern = std::get_if<PatternRule>(&sequence_option))
		{
			return check_rule_pattern(*pPattern, input);
		}
		auto pSequence = std::get_if<Sequence>(&sequence_option);
		assert(pSequence != nullptr);
		return check_rule_sequence(rules, *pSequence, input);
	}

	RuleCheckResult check_calling_rule(const RuleSet& rules, const CallingRule& calling_rule, std::string_view input)
	{
		RuleCheckResult result;
		for (const SequenceOption& sequence_option : calling_rule)
		{
			const auto sequence_result = check_rule_sequence_option(rules, sequence_option, input);
			std::copy(begin(sequence_result), end(sequence_result), std::back_inserter(result));
		}
		return result;
	}
	
	RuleCheckResult check_rule_by_id(const RuleSet& rules, RuleID id, std::string_view input)
	{
		assert(id < rules.size());
		const Rule& rule = rules[id];
		if (auto pPattern = get_as_pattern_rule(rule))
		{
			return check_rule_pattern(*pPattern, input);
		}
		else if (auto pCalling = get_as_calling_rule(rule))
		{
			return check_calling_rule(rules, *pCalling, input);
		}
		else
		{
			assert(false);
			return RuleCheckResult{};
		}
	}

	bool check_rule(const RuleSet& rules, RuleID id, std::string_view input)
	{
		const auto result = check_rule_by_id(rules, id, input);
		const bool passes = !result.empty() && std::any_of(begin(result), end(result),
			[](std::string_view r) {return r.empty(); });
#if DAY19DEBUG
		std::cout << "    \"" << input << "\": " << (passes ? "PASS" : "FAIL") << '\n';
#endif
		return passes;
	}

	auto solve_generic(const RuleSet& rules, RuleID initial_id,std::istream& input)
	{
		auto line_passes = [&rules,initial_id](std::string_view line)
		{
			return check_rule(rules, 0, line);
		};
		return std::count_if(
			utils::istream_line_iterator{ input },
			utils::istream_line_iterator{},
			line_passes);
	}

	auto solve_p1(std::istream& input)
	{
		const RuleSet rules = extract_ruleset(input);
		return solve_generic(rules, 0, input);
	}
}

ResultType day_nineteen_testcase_a()
{
	auto input = utils::open_testcase_input(19, 'a');
	return solve_p1(input);
}

ResultType advent_nineteen_p1()
{
	auto input = utils::open_puzzle_input(19);
	return solve_p1(input);
}

namespace
{
	auto solve_p2(std::istream& input)
	{
		RuleSet rules = extract_ruleset(input);
		assert(rules.size() >= 42);
		rules[8] = parse_rule("42 | 42 8");
		rules[11] = parse_rule("42 31 | 42 11 31");
		return solve_generic(rules, 0, input);
	}
}

ResultType day_nineteen_testcase_b()
{
	auto input = utils::open_testcase_input(19, 'b');
	return  solve_p2(input);
}

ResultType advent_nineteen_p2()
{
	auto input = utils::open_puzzle_input(19);
	return solve_p2(input);
}
