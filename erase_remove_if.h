#pragma once

#include <algorithm>

namespace
{

	template <typename Cont, typename Pred>
	inline void erase_remove_if(Cont& c, Pred p)
	{
		c.erase(std::remove_if(begin(c), end(c), p), end(c));
	}

}