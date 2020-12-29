#include "../advent/advent14.h"
#include "../utils/advent_utils.h"
#include "../utils/split_string.h"
#include "../utils/trim_string.h"
#include "../utils/istream_line_iterator.h"
#include "../utils/int_range.h"
#include "../utils/to_value.h"

#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <numeric>

namespace
{
	using namespace utils;
	using Register_t = uint64_t;
	using Memory_t = std::unordered_map<Register_t, Register_t>;

	constexpr uint8_t NUM_VALID_BITS = 36;
	constexpr Register_t VALID_BITS_MASK = []() {
		Register_t result = 0;
		for (std::size_t i = 0 ; i < NUM_VALID_BITS ; ++i)
		{
			result = result << 1;
			result = result | 1u;
		}
		return result;
	}();

	constexpr void validate_value(Register_t val)
	{
#ifndef NDEBUG
		const Register_t result = val & ~VALID_BITS_MASK;
		assert(result == 0u);
#endif
	}

	constexpr Register_t set_bit(Register_t original, uint8_t index, int value)
	{
		assert(index < NUM_VALID_BITS);
		assert(value == 0 || value == 1);
		validate_value(original);
		const Register_t mask = Register_t{ 1 } << index;
		Register_t result = 0;
		if (value == 0)
		{
			result = original & ~mask;
		}
		else
		{
			result = original | mask;
		}
		validate_value(result);
		return result;
	}

	struct MaskState
	{
		Register_t one_mask = 0; // Default 0. OR with me.
		Register_t zero_mask = VALID_BITS_MASK; // Default 1. AND with me.
		void validate() const
		{
			validate_value(one_mask);
			validate_value(zero_mask);
			assert((one_mask & zero_mask) == one_mask);
			assert((~zero_mask & ~one_mask) == ~zero_mask);
		}
	};

	class Program
	{
		std::vector<MaskState> masks;
		Memory_t memory;
		MaskState mask;

		void assert_valid(Register_t value) const
		{
			validate_value(value);
			mask.validate();
			for (const auto& m : masks)
			{
				m.validate();
			}
		}

		static Register_t apply_mask(Register_t value, const MaskState& mask)
		{
			validate_value(value);
			const Register_t one_masked = value | mask.one_mask;
			const Register_t result = one_masked & mask.zero_mask;
			return result;
		}

		struct SetMaskResult
		{
			MaskState mask;
			std::vector<uint8_t> float_indices;
		};

		static SetMaskResult execute_mask_generic(std::string_view new_mask)
		{
			assert(new_mask.size() == NUM_VALID_BITS);

			SetMaskResult result;
			for (auto i : int_range(NUM_VALID_BITS))
			{
				const auto index = NUM_VALID_BITS - i - 1;
				const char c = new_mask[i];
				switch (c)
				{
				case '0':
					result.mask.zero_mask = set_bit(result.mask.zero_mask, index, 0);
					break;
				case '1':
					result.mask.one_mask = set_bit(result.mask.one_mask, index, 1);
					break;
				case 'X':
					result.float_indices.push_back(index);
					break;
				default:
					assert(false);
				}
			}

			result.mask.validate();
			return result;
		}

		void execute_mask_v1(std::string_view new_mask)
		{
			const auto result = execute_mask_generic(new_mask);
			mask = result.mask;
		}

		template <typename FwdIt>
		static void get_mask_permutations(std::vector<MaskState>& result, FwdIt first, FwdIt last, const MaskState& base)
		{
			if (first == last)
			{
				result.push_back(base);
				return;
			}
			const uint8_t index = *first;
			std::advance(first, 1);
			assert(index < NUM_VALID_BITS);
			{
				MaskState hi = base;
				hi.one_mask = set_bit(hi.one_mask, index, 1);
				get_mask_permutations(result, first, last, hi);
			}
			{
				MaskState lo = base;
				lo.zero_mask = set_bit(lo.zero_mask, index, 0);
				get_mask_permutations(result, first, last, lo);
			}
		}

		static std::vector<MaskState> get_mask_permutations(const std::vector<uint8_t>& indices)
		{
			std::vector<MaskState> result;
			if (indices.empty())
			{
				return result;
			}
			const auto expected_size = 1u << indices.size();

			result.reserve(expected_size);
			get_mask_permutations(result, begin(indices), end(indices), MaskState{});

			assert(result.size() == expected_size);
			return result;
		}

		void execute_mask_v2(std::string_view new_mask)
		{
			const auto result = execute_mask_generic(new_mask);
			mask = result.mask;
			masks = get_mask_permutations(result.float_indices);
		}

		// Use after bitflipping jiggery pokery.
		void set_memory_at(Register_t location, Register_t value)
		{
			if (value == 0u)
			{
				memory.erase(location);
			}
			else
			{
				memory.insert_or_assign(location, value);
			}
		}

		void execute_mem_v1(Register_t address, Register_t arg)
		{
			const Register_t processed_arg = apply_mask(arg,mask);
			set_memory_at(address, processed_arg);
		}

		void execute_mem_v2(Register_t address, Register_t arg)
		{
			const Register_t preprocessed_address = address | mask.one_mask;
			for (const MaskState& ms : masks)
			{
				const Register_t processed_address = apply_mask(preprocessed_address, ms);
				set_memory_at(processed_address, arg);
			}
		}

	public:
		void execute_line(const std::string& line, int version)
		{
			assert(version == 1 || version == 2);
			const auto [command, arg] = [&line]()
			{
				auto result = split_string(line,'=');
				assert(result.size() == 2);

				return std::make_pair(std::string{ trim_string(result[0]) }, std::string{ trim_string(result[1]) });
			}();

			if (command == "mask")
			{
				switch (version)
				{
				case 1:
					execute_mask_v1(arg);
					break;
				case 2:
					execute_mask_v2(arg);
					break;
				default:
					assert(false);
					break;
				}
			}
			else if (command.starts_with("mem"))
			{
				std::string_view address(command);
				assert(address.back() == ']');
				address.remove_prefix(4);
				address.remove_suffix(1);
				const Register_t address_val = to_value<Register_t>(address);
				const Register_t arg_val = to_value<Register_t>(arg);
				switch (version)
				{
				case 1:
					execute_mem_v1(address_val, arg_val);
					break;
				case 2:
					execute_mem_v2(address_val, arg_val);
					break;
				default:
					assert(false);
				}
			}
		}

		Register_t get_sum_of_memory() const
		{
			return std::transform_reduce(begin(memory), end(memory), Register_t{ 0u },
				std::plus<Register_t>{}, [](const Memory_t::value_type& v) {return v.second; });
		}
	};

	Register_t solve_generic(std::istream& input, int version)
	{
		assert(version == 1 || version == 2);
		Program prog;
		std::for_each(istream_line_iterator(input), istream_line_iterator(), [&prog,version](const std::string& s)
		{
			prog.execute_line(s, version);
		});
		return prog.get_sum_of_memory();
	}

	Register_t solve_p1(std::istream& input)
	{
		return solve_generic(input,1);
	}

	Register_t solve_p2(std::istream& input)
	{
		return solve_generic(input, 2);
	}

	std::istringstream get_testcase_input_a()
	{
		return std::istringstream{
			"mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X\n"
			"mem[8] = 11\n"
			"mem[7] = 101\n"
			"mem[8] = 0"
		};
	}

	std::istringstream get_testcase_input_b()
	{
		return std::istringstream{
			"mask = 000000000000000000000000000000X1001X\n"
			"mem[42] = 100\n"
			"mask = 00000000000000000000000000000000X0XX\n"
			"mem[26] = 1"
		};
	}
}

ResultType day_fourteen_testcase_a()
{
	auto input = get_testcase_input_a();
	return solve_p1(input);
}

ResultType advent_fourteen_p1()
{
	auto input = open_puzzle_input(14);
	return solve_p1(input);
}

ResultType day_fourteen_testcase_b()
{
	auto input = get_testcase_input_b();
	return solve_p2(input);
}

ResultType advent_fourteen_p2()
{
	auto input = open_puzzle_input(14);
	return solve_p2(input);
}