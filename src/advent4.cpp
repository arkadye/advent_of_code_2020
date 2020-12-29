#include "../advent/advent4.h"
#include "../utils/advent_utils.h"
#include "../utils/in_range.h"

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cassert>
#include <functional>

namespace
{
	using Passport = std::map<std::string, std::string>;
	using utils::open_puzzle_input;
	using utils::open_testcase_input;
	
	static const std::vector<std::string> REQUIRED_TAGS_P1 {
		"byr",
		"iyr",
		"eyr",
		"hgt",
		"hcl",
		"ecl",
		"pid"
	};

	bool verify_passport(const Passport& passport, const std::vector<std::string>& required_tags)
	{
		return std::all_of(begin(required_tags), end(required_tags),
			[&passport](const std::string& tag) {return passport.find(tag) != end(passport); });
	}

	int solve_p1_generic(const std::vector<Passport>& passports, const std::vector<std::string>& required_tags)
	{
		return std::count_if(begin(passports), end(passports),
			[&required_tags](const Passport& pp) {return verify_passport(pp, required_tags); });
	}

	Passport get_passport_from_string(std::string input)
	{
		std::istringstream iss{ std::move(input) };
		auto get_entry = [](const std::string& field)
		{
			return Passport::value_type(field.substr(0,3),field.substr(4));
		};
		
		Passport result;
		for (auto it = std::istream_iterator<std::string>(iss); it != std::istream_iterator<std::string>(); ++it)
		{
			auto field = get_entry(*it);
			assert(!result.contains(field.first));
			result.insert(std::move(field));
		}
		return result;
	}

	std::vector<Passport> get_passports(std::istream& input)
	{
		std::vector<Passport> result;
		while (!input.eof())
		{
			std::ostringstream oss;
			std::size_t line_size = 1;
			while(line_size != 0)
			{
				std::string line;
				std::getline(input, line);
				line_size = line.size();
				oss << std::move(line) << ' ';
			}
			result.push_back(get_passport_from_string(oss.str()));
		}
		return result;
	}

	ResultType solve_p1(std::istream& input)
	{
		return solve_p1_generic(get_passports(input), REQUIRED_TAGS_P1);
	}
}

ResultType day_four_p1_testcase_a()
{
	std::ifstream input = open_testcase_input(4, 'a');
	return solve_p1(input);
}

ResultType advent_four_p1()
{
	std::ifstream input = open_puzzle_input(4);
	return solve_p1(input);
}

namespace
{
	bool verify_text_number(const std::string& field, int min, int max)
	{
		if (!std::all_of(begin(field), end(field), [](char c) {return std::isdigit(c); })) return false;
		const int val = std::stoi(field);
		return utils::in_range(val, min, max);
	}

	bool verify_byr(const std::string& field)
	{
		return verify_text_number(field, 1920, 2002);
	}

	bool verify_iyr(const std::string& field)
	{
		return verify_text_number(field, 2010, 2020);
	}

	bool verify_eyr(const std::string& field)
	{
		return verify_text_number(field, 2020, 2030);
	}

	bool verify_hgt(const std::string& field)
	{
		const auto split_point = field.find_first_of("ic");
		if (split_point >= field.size()) return false;
		const std::string number = field.substr(0, split_point);
		const std::string suffix = field.substr(split_point);
		if (suffix == "cm")
		{
			return verify_text_number(number, 150, 193);
		}
		else if (suffix == "in")
		{
			return verify_text_number(number, 59, 76);
		}
		return false;
	}

	bool verify_hcl(const std::string& field)
	{
		if (field.size() != 7u) return false;
		return field[0] == '#' && std::all_of(begin(field) + 1, end(field), [](char c)
		{
			return isdigit(c) || utils::in_range(c, 'a', 'f');
		});
	}

	bool verify_ecl(const std::string& field)
	{
		static const std::vector<std::string> allowed_values = {
			"amb", "blu", "brn", "gry", "grn", "hzl", "oth"
		};
		return std::find(begin(allowed_values), end(allowed_values), field) != end(allowed_values);
	}

	bool verify_pid(const std::string& field)
	{
		return field.size() == 9u && std::all_of(begin(field), end(field), ::isdigit);
	}

	bool verify_cid(const std::string& field)
	{
		return true;
	}

	bool verify_field(const Passport::value_type& input)
	{

		static const std::map<std::string, std::function<bool(const std::string&)>> verify_map{
			{"byr",verify_byr},
			{"iyr",verify_iyr},
			{"eyr",verify_eyr},
			{"hgt",verify_hgt},
			{"hcl",verify_hcl},
			{"ecl",verify_ecl},
			{"pid",verify_pid},
			{"cid",verify_cid}
		};

		auto find_result = verify_map.find(input.first);
		assert(find_result != end(verify_map));
		return find_result->second(input.second);
	}

	bool verify_passport_and_fields(const Passport& passport, const std::vector<std::string>& required_tags)
	{
		return verify_passport(passport, required_tags) && std::all_of(begin(passport), end(passport), verify_field);
	}

	int solve_p2_generic(const std::vector<Passport>& passports, const std::vector<std::string>& required_tags)
	{
		return std::count_if(begin(passports), end(passports),
			[&required_tags](const Passport& p) { return verify_passport_and_fields(p, required_tags); });
	}

	ResultType solve_p2(std::istream& input)
	{
		return solve_p2_generic(get_passports(input), REQUIRED_TAGS_P1);
	}
}

ResultType day_four_p2_testcase_b()
{
	std::ifstream input = open_testcase_input(4, 'b');
	return solve_p2(input);
}

ResultType day_four_p2_testcase_c()
{
	std::ifstream input = open_testcase_input(4, 'c');
	return solve_p2(input);
}

ResultType advent_four_p2()
{
	auto input = open_puzzle_input(4);
	return solve_p2(input);
}
