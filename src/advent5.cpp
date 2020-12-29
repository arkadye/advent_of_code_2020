#include "../advent/advent5.h"
#include "../utils/advent_utils.h"
#include "../utils/sorted_vector.h"

#include <string>
#include <numeric>
#include <cassert>

namespace
{
	using FileIt = std::istream_iterator<std::string>;
	using utils::open_puzzle_input;
	using utils::sorted_vector;

	constexpr char FORWARD = 'F';
	constexpr char BACKWARD = 'B';
	constexpr char LEFT = 'L';
	constexpr char RIGHT = 'R';

	template <typename InputIt>
	int partition(InputIt pass_start, InputIt pass_finish, int low_end, int top_end, char go_low, char go_high)
	{
		if (pass_start == pass_finish)
		{
			assert(low_end == top_end || low_end == (top_end -1));
			return low_end;
		}
		const char section = *pass_start;
		assert(section == go_low || section == go_high);

		auto recurse = [&](int new_lower, int new_upper)
		{
			return partition(pass_start + 1, pass_finish, new_lower, new_upper, go_low, go_high);
		};

		const int midpoint = low_end + ((top_end - low_end) / 2);
		if (section == go_low)
		{
			return recurse(low_end, midpoint);
		}
		else if (section == go_high)
		{
			return recurse(midpoint, top_end);
		}
		assert(false);
		return -1;
	}

	template <typename InputIt>
	int partition(InputIt start, InputIt finish, int max_val, char go_low, char go_high)
	{
		return partition(start, finish, 0, max_val, go_low, go_high);
	}

	int get_seat_number_generic(const std::string& id, int row_digits, int col_digits)
	{
		assert(static_cast<int>(id.size()) == (row_digits + col_digits));
		const int num_rows = 1 << row_digits;
		const int num_cols = 1 << col_digits;

		const auto split_point = begin(id) + row_digits;
		const int row_number = partition(begin(id), split_point, num_rows, FORWARD, BACKWARD);
		const int col_number = partition(split_point, end(id), num_cols, LEFT, RIGHT);
		return row_number * num_cols + col_number;
	}

	int get_seat_number_p1(const std::string& id)
	{
		return get_seat_number_generic(id, 7, 3);
	}
}

ResultType day_five_testcase_a()
{
	return get_seat_number_p1("FBFBBFFRLR");
}

ResultType day_five_testcase_b()
{
	return get_seat_number_p1("BFFFBBFRRR");
}

ResultType day_five_testcase_c()
{
	return get_seat_number_p1("FFFBBBFRRR");
}

ResultType day_five_testcase_d()
{
	return get_seat_number_p1("BBFFBBFRLL");
}


ResultType advent_five_p1()
{
	std::ifstream input = open_puzzle_input(5);
	return std::transform_reduce(FileIt{ input }, FileIt{},
		-1, [](int l, int r) {return std::max(l, r); }, get_seat_number_p1);
}

ResultType advent_five_p2()
{
	const sorted_vector<int> ids = []()
	{
		sorted_vector<int> result;
		std::ifstream input = open_puzzle_input(5);
		std::for_each(FileIt{ input }, FileIt{}, [&result](const std::string& s) {result.insert(get_seat_number_p1(s)); });
		return result;
	}();
	for (std::size_t i = 0; i < (ids.size() - 1); ++i)
	{
		const auto this_one = ids[i];
		const auto next_one = ids[i + 1];
		const auto difference = next_one - this_one;
		if (difference == 2)
		{
			return this_one + 1;
		}
		assert(difference == 1);
	}
	assert(false);
	return "!ERROR!";

}
