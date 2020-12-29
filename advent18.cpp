#include "../advent/advent18.h"
#include "../utils/advent_utils.h"

#include "../utils/trim_string.h"
#include "../utils/int_range.h"
#include "../utils/to_value.h"
#include "../utils/istream_line_iterator.h"

#include <string>
#include <string_view>
#include <array>
#include <optional>
#include <algorithm>
#include <cassert>
#include <vector>
#include <numeric>

namespace
{
	enum class Operator : char
	{
		add = '+',
		mul = '*'
	};

	// Low to high
	std::string_view get_operators_in_precedence_order()
	{
		static const std::string ops{ "*+" };
		return std::string_view(ops);
	}

	bool is_operator(char c)
	{
		return get_operators_in_precedence_order().find(c) != std::string_view::npos;
	}

	Operator to_operator(char c)
	{
		assert(is_operator(c));
		return static_cast<Operator>(c);
	}

	using ValType = int64_t;

	void verify_brackets(std::string_view expression)
	{
		const auto open_result = expression.find_first_of('(');
		const auto close_result = expression.find_last_of(')');
		if (open_result < expression.size() || close_result < expression.size())
		{
			assert(open_result < close_result);
			auto count_vals = [&expression](char c)
			{
				return std::count(begin(expression), end(expression), c);
			};
			assert(count_vals('(') == count_vals(')'));
		}
	}

	ValType parse_expression(const std::string_view& expression, bool use_operator_precedence);

	std::size_t get_last_out_of_bracket(const std::string_view& exp)
	{
		assert(!exp.empty());
		int num_open_brackets = 0;
		const auto range = utils::int_range<int>(exp.size());
		for (auto it = rbegin(range);it!=rend(range);++it)
		{
			assert(num_open_brackets >= 0);
			const auto i = *it;
			const char c = exp[i];
			if (c == ')') ++num_open_brackets;
			else if (c == '(') --num_open_brackets;
			else if (num_open_brackets == 0) return i;
		}
		return std::string_view::npos;
	}

	std::size_t get_active_operator_no_precedence(const std::string_view& exp)
	{
		assert(!exp.empty());
		const auto range = utils::int_range(exp.size());
		for (auto it = rbegin(range); it != rend(range); ++it)
		{
			const auto i = *it;
			const char c = exp[i];
			if (is_operator(c))
			{
				return i;
			}
			assert(std::string_view("()").find(c) == std::string_view::npos);
			assert(isdigit(c) || isspace(c));
		}
		return std::string_view::npos;
	}

	std::size_t get_active_operator_with_precedence(const std::string_view& exp)
	{
		assert(!exp.empty());
		for (auto op : get_operators_in_precedence_order())
		{
			const auto range = utils::int_range(exp.size());
			int num_open_brackets = 0;
			for (auto it = rbegin(range); it != rend(range); ++it)
			{
				assert(num_open_brackets >= 0);
				const auto i = *it;
				const char c = exp[i];
				if (c == ')') ++num_open_brackets;
				else if (c == '(') --num_open_brackets;
				else if (num_open_brackets == 0 && c == op)
				{
					return i;
				}
			}
		}
		return std::string_view::npos;
	}

	ValType parse_trimmed_expression(std::string_view expression, bool use_operator_precedence)
	{
		assert(!expression.empty());
		verify_brackets(expression);
		const std::size_t last_non_bracketed = get_last_out_of_bracket(expression);
		if (last_non_bracketed == std::string_view::npos) // Whole thing is in brackets.
		{
			assert(expression.front() == '(');
			assert(expression.back() == ')');
			expression.remove_prefix(1);
			expression.remove_suffix(1);
			return parse_expression(expression, use_operator_precedence);
		}

		{
			const std::string_view sub_expr = expression.substr(0, last_non_bracketed+1);
			const std::size_t op_index = use_operator_precedence
				? get_active_operator_with_precedence(sub_expr)
				: get_active_operator_no_precedence(sub_expr);

			if (op_index != std::string_view::npos)
			{
				const Operator op = to_operator(expression[op_index]);
				const auto lhs = expression.substr(0, op_index);
				const auto rhs = expression.substr(op_index + 1);
				const ValType left = parse_trimmed_expression(utils::trim_right(lhs),use_operator_precedence);
				const ValType right = parse_trimmed_expression(utils::trim_left(rhs),use_operator_precedence);
				switch (op)
				{
				case Operator::add:
					return left + right;
				case Operator::mul:
					return left * right;
				default:
					assert(false);
					break;
				}
				return 0;
			}

		}

		// No brackets and no operators found.  So it must be a value.
		return utils::to_value<ValType>(expression);
	}

	ValType parse_expression(const std::string_view& expression, bool use_operator_precedence)
	{
		return parse_trimmed_expression(utils::trim_string(expression),use_operator_precedence);
	}
}

ResultType day_eighteen_testcase_a()
{
	return parse_expression("1 + 2 * 3 + 4 * 5 + 6",false);
}

ResultType day_eighteen_testcase_b()
{
	return parse_expression("1 + (2 * 3) + (4 * (5 + 6))",false);
}

ResultType day_eighteen_testcase_c()
{
	return parse_expression("2 * 3 + (4 * 5)",false);
}

ResultType day_eighteen_testcase_d()
{
	return parse_expression("5 + (8 * 3 + 9 + 3 * 4 * 3)",false);
}

ResultType day_eighteen_testcase_e()
{
	return parse_expression("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))",false);
}

ResultType day_eighteen_testcase_f()
{
	return parse_expression("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2",false);
}

ResultType advent_eighteen_p1()
{
	using ItType = utils::istream_line_iterator;
	auto input = utils::open_puzzle_input(18);
	auto parse = [](std::string_view line) {return parse_expression(line, false); };
	return std::transform_reduce(ItType{ input }, ItType{}, ValType{ 0 }, std::plus<ValType>{}, parse);
}

ResultType day_eighteen_testcase_g()
{
	return parse_expression("1 + 2 * 3 + 4 * 5 + 6", true);
}

ResultType day_eighteen_testcase_h()
{
	return parse_expression("1 + (2 * 3) + (4 * (5 + 6))", true);
}

ResultType day_eighteen_testcase_i()
{
	return parse_expression("2 * 3 + (4 * 5)", true);
}

ResultType day_eighteen_testcase_j()
{
	return parse_expression("5 + (8 * 3 + 9 + 3 * 4 * 3)", true);
}

ResultType day_eighteen_testcase_k()
{
	return parse_expression("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))", true);
}

ResultType day_eighteen_testcase_l()
{
	return parse_expression("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2", true);
}

ResultType advent_eighteen_p2()
{
	using ItType = utils::istream_line_iterator;
	auto input = utils::open_puzzle_input(18);
	auto parse = [](std::string_view line) {return parse_expression(line, true); };
	return std::transform_reduce(ItType{ input }, ItType{}, ValType{ 0 }, std::plus<ValType>{}, parse);
}