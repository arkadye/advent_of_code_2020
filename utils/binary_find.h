#pragma once

#include <algorithm>

namespace
{

	template <typename FwdIt, typename T>
	FwdIt binary_find(FwdIt start, FwdIt finish, const T& val)
	{
		const auto result = std::lower_bound(start, finish, val);
		return (result != finish && !(*result < val) && !(val < *result)) ? result : finish;
	}

}
