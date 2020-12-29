#include "../advent/advent2.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/advent_utils.h"

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cassert>
#include <fstream>

namespace
{
	struct PasswordData
	{
		std::string password;
		int min_occurance = 0;
		int max_occurance = 0;
		char reference = 0;
	};

	bool validate_password_p1(const PasswordData& pd)
	{
		const int ref_count = std::count(begin(pd.password), end(pd.password), pd.reference);
		return (pd.min_occurance <= ref_count && ref_count <= pd.max_occurance);
	}

	bool validate_password_p2(const PasswordData& pd)
	{
		const bool min_has_ref = pd.password[pd.min_occurance - 1] == pd.reference;
		const bool max_has_ref = pd.password[pd.max_occurance - 1] == pd.reference;
		return min_has_ref != max_has_ref;
	}

	ResultType solve_p1_generic(const std::vector<PasswordData>& passwords)
	{
		const auto result = std::count_if(begin(passwords), end(passwords), validate_password_p1);
		return result;
	}

	ResultType solve_p2_generic(const std::vector<PasswordData>& passwords)
	{
		const auto result = std::count_if(begin(passwords), end(passwords), validate_password_p2);
		return result;
	}

	PasswordData parse_database_entry(std::string entry)
	{
		std::istringstream iss{ std::move(entry) };
		PasswordData result;
		char hyphen = 0; // Unused.
		std::string ref_char_str;
		iss >> result.min_occurance >> hyphen >> result.max_occurance >> ref_char_str >> result.password;
		assert(result.min_occurance <= result.max_occurance);
		assert(ref_char_str.size() == 2);
		assert(ref_char_str[1] == ':');
		assert(hyphen == '-');
		result.reference = ref_char_str[0];
		return result;
	}

	std::vector<PasswordData> get_db_from_stream(std::istream& input)
	{
		std::vector<PasswordData> result;
		std::transform(
			utils::istream_line_iterator(input),
			utils::istream_line_iterator(),
			std::back_inserter(result),
			parse_database_entry);
		return result;
	}

	std::vector<PasswordData> get_testcase_input()
	{
		std::istringstream iss{
			"1-3 a: abcde\n"
			"1-3 b: cdefg\n"
			"2-9 c: ccccccccc"
		};
		return get_db_from_stream(iss);
	}

	std::vector<PasswordData> get_puzzle_input()
	{
		std::ifstream input = utils::open_puzzle_input(2);
		return get_db_from_stream(input);
	}
}

ResultType day_two_p1_testcase()
{
	return solve_p1_generic(get_testcase_input());
}

ResultType day_two_p2_testcase()
{
	return solve_p2_generic(get_testcase_input());
}

ResultType advent_two_p1()
{
	return solve_p1_generic(get_puzzle_input());
}

ResultType advent_two_p2()
{
	return solve_p2_generic(get_puzzle_input());
}
