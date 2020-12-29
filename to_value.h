#pragma once

#include <charconv>
#include <string_view>

namespace utils
{
	template <typename T>
	inline T to_value(const std::string_view& sv)
	{
		const char* first = sv.data();
		const char* last = first + sv.size();
		T value{};
		const std::from_chars_result result = std::from_chars(first, last, value);
		assert(result.ec == std::errc{});
		assert(result.ptr == last);
		return value;
	}
}