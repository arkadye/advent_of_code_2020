#pragma once

// This has a terrible name and will be renamed to something sensible in the next iteration.

namespace utils
{
	template <typename T> 
	inline bool in_range(T val, T min, T max)
	{
		return min <= val && val <= max;
	}

}
