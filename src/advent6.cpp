#include "../advent/advent6.h"
#include "../utils/advent_utils.h"
#include "../utils/istream_line_iterator.h"

#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

namespace
{
	using utils::open_testcase_input;
	using utils::open_puzzle_input;

	class QuestionairreSolver
	{
		std::string group_answers;
		bool addidative = true;

		int solve_addidative(const std::string& str)
		{
			std::string result;
			std::set_union(begin(group_answers), end(group_answers), begin(str), end(str), std::back_inserter(result));
			group_answers = std::move(result);
			return 0;
		}

		int solve_subtractive(const std::string& str)
		{
			std::string result;
			std::set_intersection(begin(group_answers), end(group_answers), begin(str), end(str), std::back_inserter(result));
			group_answers = std::move(result);
			return 0;
		}

		void reset()
		{
			group_answers = addidative ? "" : "abcdefghijklmnopqrstuvwxyz";
		}

	public:
		void set_subtractive()
		{
			addidative = false;
			reset();
			
		}
		int add_line(std::string str)
		{
			if (str.empty())
			{
				const int result = static_cast<int>(group_answers.size());
				reset();
				return result;
			}

			std::sort(begin(str), end(str));
			return addidative ? solve_addidative(str) : solve_subtractive(str);
		}
	};

	int solve_generic(std::istream& file, bool addidative)
	{
		using StreamIt = utils::istream_line_iterator;
		QuestionairreSolver solver;
		if (!addidative)
		{
			solver.set_subtractive();
		}
		auto process_line = [&solver](std::string s)
		{
			return solver.add_line(std::move(s));
		};
		auto combine_results = [](int a, int b) { return a + b; };
		const int result = std::transform_reduce(StreamIt(file), StreamIt(), 0, combine_results, process_line);
		return result + solver.add_line("");
	}

	int solve_p1_generic(std::istream& file)
	{
		return solve_generic(file, true);
	}

	int solve_p2_generic(std::istream& file)
	{
		return solve_generic(file, false);
	}

	std::ifstream get_testcase_input()
	{
		return open_testcase_input(6, 'a');
	}
}

ResultType day_six_testcase_a()
{
	std::ifstream input = get_testcase_input();
	return solve_p1_generic(input);
}

ResultType advent_six_p1()
{
	std::ifstream input = open_puzzle_input(6);
	return solve_p1_generic(input);
}

ResultType day_six_testcase_b()
{
	std::ifstream input = get_testcase_input();
	return solve_p2_generic(input);
}

ResultType advent_six_p2()
{
	std::ifstream input = open_puzzle_input(6);
	return solve_p2_generic(input);
}
