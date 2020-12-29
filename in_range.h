#pragma once

namespace utils
{
	template <typename T> 
	inline bool in_range(T val, T min, T max)
	{
		return min <= val && val <= max;
	}

}