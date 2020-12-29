#include "../advent/advent25.h"
#include "../utils/int_range.h"

#include <numeric>

namespace
{
	using utils::int_range;
	using ValType = int64_t;
	constexpr ValType MOD = 20201227;
	constexpr ValType CARD_SUBJECT_NUMBER = 7;
	constexpr ValType DOOR_SUBJECT_NUMBER = 7;
	constexpr ValType INITIAL_CRYPTO_VALUE = 1;

	ValType update_crypto_value(ValType value, ValType subject_number)
	{
		const ValType intermediate = value * subject_number;
		const ValType result = intermediate % MOD;
		return result;
	}

	ValType encrypt(ValType subject_number, ValType loop_size)
	{
		const int_range range(loop_size);
		return std::accumulate(begin(range), end(range), INITIAL_CRYPTO_VALUE,
			[subject_number](ValType prev, ValType)
		{
			return update_crypto_value(prev, subject_number);
		});
	}

	ValType get_loop_size(ValType subject_number, ValType public_key)
	{
		ValType crypto_value = INITIAL_CRYPTO_VALUE;
		ValType trial_loop_size = 0;
		while (true)
		{
			if (crypto_value == public_key) return trial_loop_size;
			++trial_loop_size;
			crypto_value = update_crypto_value(crypto_value, subject_number);
		}
	}

	ValType get_encryption_key(ValType my_public, ValType other_public, ValType my_subject_number)
	{
		const ValType my_loop_size = get_loop_size(my_subject_number, my_public);
		return encrypt(other_public, my_loop_size);
	}

	ValType solve_p1(ValType card_key, ValType door_key)
	{
		const auto result = get_encryption_key(card_key, door_key, CARD_SUBJECT_NUMBER);
		assert(result == get_encryption_key(door_key, card_key, DOOR_SUBJECT_NUMBER));
		return result;
	}
}

ResultType day_twentyfive_testcase_a()
{
	return solve_p1(5764801, 17807724);
}

ResultType advent_twentyfive_p1()
{
	return solve_p1(1327981, 2822615);
}

ResultType advent_twentyfive_p2()
{
	return "MERRY CHRISTMAS!";
}