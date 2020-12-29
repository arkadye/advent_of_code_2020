#include "../advent/advent16.h"
#include "../utils/advent_utils.h"
#include "../utils/in_range.h"
#include "../utils/split_string.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/swap_remove.h"
#include "../utils/int_range.h"
#include "../utils/to_value.h"

#include <vector>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <iostream>

namespace
{
	using ValType = int64_t;
	using TicketNoNames = std::vector<ValType>;
	using Range = std::pair<ValType, ValType>;
	using Constraint = std::pair<Range,Range>;
	using TicketData = std::unordered_map<std::string, Constraint>;

	bool validate_value(ValType val, const Range& range)
	{
		return utils::in_range(val, range.first, range.second);
	}

	bool validate_value(ValType val, const Constraint& constraint)
	{
		return validate_value(val,constraint.first) || validate_value(val,constraint.second);
	}

	bool is_reasonable_value(ValType val, const TicketData& td)
	{
		const auto result = std::any_of(begin(td), end(td),
			[val](const TicketData::value_type& constraint) {return validate_value(val, constraint.second); });
		return result;
	}

	ValType sum_invalid_values(const TicketNoNames& ticket, const TicketData& td)
	{
		return std::transform_reduce(begin(ticket), end(ticket), ValType{ 0 },
			std::plus<ValType>{}, [&td](ValType val) {return is_reasonable_value(val, td) ? 0 : val; });
	}

	Range extract_range(std::string_view input)
	{
		const auto split_range = utils::split_string(input, '-');
		assert(split_range.size() == 2);
		auto is_digits = [](std::string_view str)
		{
			return std::all_of(begin(str), end(str), ::isdigit);
		};
		assert(is_digits(split_range[0]));
		assert(is_digits(split_range[1]));
		return std::make_pair(utils::to_value<ValType>(split_range[0]), utils::to_value<ValType>(split_range[1]));
	}

	bool extract_ticket_data(TicketData& td, std::istream& input)
	{
		std::string line;
		std::getline(input, line);
		if (line.empty())
		{
			return false;
		}

		std::istringstream line_stream{ std::move(line) };
		std::string field_name;
		std::getline(line_stream, field_name, ':');
		assert(td.find(field_name) == end(td));

		std::string range1, says_or, range2;
		line_stream >> range1 >> says_or >> range2;
		assert(says_or == "or");

		td[field_name] = std::make_pair(extract_range(range1), extract_range(range2));

		return true;
	}

	TicketNoNames extract_ticket(std::string_view line)
	{
		auto is_digits = [](const std::string_view& str)
		{
			return std::all_of(begin(str), end(str), ::isdigit);
		};
		const auto vals = utils::split_string(line, ',');
		TicketNoNames result;
		std::transform(begin(vals), end(vals), std::back_inserter(result),
			[&is_digits](const std::string_view& s) { assert(is_digits(s)); return utils::to_value<ValType>(s); });
		return result;
	}

	TicketData extract_ticket_data(std::istream& input)
	{
		TicketData result;
		while (extract_ticket_data(result, input));
		return result;
	}

	TicketNoNames extract_my_ticket(std::istream& input)
	{
		std::string line;
		std::getline(input, line);
		assert(line == "your ticket:");
		std::getline(input, line);
		return extract_ticket(line);
	}

	void go_to_nearby_tickets(std::istream& input)
	{
		std::string line;
		std::getline(input, line);
		assert(line.empty());
		std::getline(input, line);
		assert(line == "nearby tickets:");
	}

	std::pair<TicketData,TicketNoNames> process_input_headers(std::istream& input)
	{
		TicketData td = extract_ticket_data(input);
		TicketNoNames my_ticket = extract_my_ticket(input);
		go_to_nearby_tickets(input);
		return std::make_pair(std::move(td), std::move(my_ticket));
	}

	ValType solve_p1(std::istream& input)
	{
		const auto [ticket_data, unused_dummy] = process_input_headers(input);
		auto transform_op = [&ticket_data](std::string line)
		{
			TicketNoNames t = extract_ticket(std::move(line));
			return sum_invalid_values(t, ticket_data);
		};
		return std::transform_reduce(utils::istream_line_iterator{ input }, utils::istream_line_iterator{},
			ValType{ 0 }, std::plus<ValType>{}, transform_op);
	}
}

ResultType day_sixteen_testcase_a()
{
	auto input = utils::open_testcase_input(16, 'a');
	return solve_p1(input);
}
ResultType advent_sixteen_p1()
{
	auto input = utils::open_puzzle_input(16);
	return solve_p1(input);
}


namespace
{
	using TicketNames = std::unordered_map<std::string, ValType>;

	using PossibilityMatrix = std::vector<std::vector<std::string>>;

	// Return true if this solves this particular field i.e. possibilities goes from size > 1 to size == 1.
	bool process_field_possibilities(std::vector<std::string>& possibilities, const TicketData& td, ValType val)
	{
		if (possibilities.size() == 1)
		{
			return false;
		}

		std::size_t i = 0;
		while (i < possibilities.size())
		{
			const std::string& field = possibilities[i];
			const auto constraint_it = td.find(field);
			assert(constraint_it != end(td));
			const Constraint& constraint = constraint_it->second;
			const bool is_valid = validate_value(val, constraint);

			if (is_valid)
			{
				++i;
				continue;
			}
			else
			{
				utils::swap_remove(possibilities, begin(possibilities) + i);
				assert(!possibilities.empty());
			}
		}
		return possibilities.size() == 1;
	}

	bool is_valid_ticket(const TicketData& td, const TicketNoNames& ticket)
	{
		return std::all_of(begin(ticket), end(ticket),
			[&td](ValType val)
		{
			return is_reasonable_value(val, td);
		});
	}

	void handle_single_possibilities(PossibilityMatrix& pm, std::size_t index)
	{
		assert(index < pm.size());
		assert(pm[index].size() == 1);
		const std::string& field_name = pm[index][0];

		for (auto i : utils::int_range(pm.size()))
		{
			if (i == index) continue;
			std::vector<std::string>& possibilities = pm[i];
			if (possibilities.size() == 1) continue;
			utils::swap_remove(possibilities, field_name);
			if (possibilities.size() == 1)
			{
				handle_single_possibilities(pm, i);
			}
		}
	}

	void process_ticket(PossibilityMatrix& pm, const TicketData& td, const TicketNoNames& ticket)
	{
		assert(pm.size() == td.size());
		assert(pm.size() == ticket.size());
		if (!is_valid_ticket(td, ticket))
		{
			return;
		}
		for (auto i : utils::int_range(pm.size()))
		{
			if (process_field_possibilities(pm[i], td, ticket[i]))
			{
				handle_single_possibilities(pm, i);
			}
		}
	}

	void process_ticket(PossibilityMatrix& pm, const TicketData& td, std::string ticket)
	{
		process_ticket(pm, td, extract_ticket(std::move(ticket)));
	}

	TicketNames decode_ticket(std::istream& input, const TicketData& td, const TicketNoNames& my_ticket)
	{
		assert(td.size() == my_ticket.size());
		PossibilityMatrix possibilities = [&td]()
		{
			std::vector<std::string> all_fields;
			all_fields.reserve(td.size());
			std::transform(begin(td), end(td), std::back_inserter(all_fields),
				[](const TicketData::value_type& tdv) {return tdv.first; });
			return PossibilityMatrix(td.size(), all_fields);
		}();

		std::for_each(utils::istream_line_iterator{ input }, utils::istream_line_iterator{},
			[&td, &possibilities](std::string line)
		{
			process_ticket(possibilities, td, std::move(line));
		});

		TicketNames result;
		std::transform(std::make_move_iterator(begin(possibilities)),
			std::make_move_iterator(end(possibilities)), begin(my_ticket), std::inserter(result, begin(result)),
			[](std::vector<std::string> possibilities, ValType val)
		{
			assert(possibilities.size() == 1);
			return std::make_pair(std::move(possibilities[0]), val);
		});
		return result;
	}

	bool has_prefix(std::string_view test, std::string_view prefix)
	{
		return test.starts_with(prefix);
	}

	ValType solve_p2(std::istream& input, const TicketData& td, const TicketNoNames& my_ticket, std::string_view field_prefix)
	{
		const TicketNames ticket = decode_ticket(input, td, my_ticket);

		assert(6 == std::count_if(begin(ticket), end(ticket),
			[&field_prefix](const TicketNames::value_type& tnv)
		{
			return has_prefix(tnv.first, field_prefix);
		}));

		auto transform_op = [&field_prefix](const TicketNames::value_type& tval)
		{
			return has_prefix(tval.first, field_prefix) ? tval.second : 1;
		};

		return std::transform_reduce(begin(ticket), end(ticket), ValType{ 1 }, std::multiplies<ValType>{}, transform_op);
	}
}

ResultType day_sixteen_testcase_b()
{
	auto input = utils::open_testcase_input(16,'b');
	const auto [td, my_ticket] = process_input_headers(input);
	const TicketNames decoded = decode_ticket(input, td, my_ticket);
	std::vector<std::pair<std::string,ValType>> result_data{ begin(decoded),end(decoded) };
	std::sort(begin(result_data), end(result_data));
	std::ostringstream result_oss;
	std::transform(begin(result_data), end(result_data), std::ostream_iterator<std::string>{result_oss},
		[](const TicketNames::value_type& tnvt)
	{
		std::ostringstream oss;
		oss << tnvt.first << '=' << tnvt.second << ' ';
		return oss.str();
	});

	std::string result = result_oss.str();
	assert(result.back() == ' ');
	result.pop_back();
	return result;
}

ResultType advent_sixteen_p2()
{
	auto input = utils::open_puzzle_input(16);
	const auto [td, my_ticket] = process_input_headers(input);
	return solve_p2(input, td, my_ticket, "departure");
}
