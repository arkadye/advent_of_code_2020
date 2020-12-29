#pragma once

#include <vector>

namespace utils
{
	template <typename VecType>
	void swap_remove(VecType& vector, typename VecType::iterator loc)
	{
		*loc = vector.back();
		vector.pop_back();
	}

	// Returns true if it successfully removes a value.
	template <typename VecType>
	bool swap_remove(VecType& vector, typename const VecType::value_type& value)
	{
		const auto loc = std::find(begin(vector), end(vector), value);
		if (loc != cend(vector))
		{
			swap_remove(vector, loc);
			return true;
		}
		return false;
	}
}
