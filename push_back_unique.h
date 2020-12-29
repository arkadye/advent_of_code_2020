#pragma once

#include <algorithm>

namespace utils
{

	template <typename Container, typename ElemType>
	bool push_back_unique(Container& cont, ElemType&& elem)
	{
		const auto find_result = std::find(begin(cont), end(cont), elem);
		if (find_result == end(cont))
		{
			cont.push_back(std::forward<ElemType>(elem));
			return true;
		}
		return false;
	}
}