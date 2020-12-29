#pragma once

namespace utils
{
	template <typename FwdIt, typename OutputIt, typename Transform, typename Pred>
	OutputIt transform_if(FwdIt start, FwdIt end, OutputIt destination, Transform transform_func, Pred predicate_func)
	{
		for (auto it = start; it != end; ++it)
		{
			const auto& value = *it;
			if (predicate_func(value))
			{
				*destination = transform_func(value);
				++destination;
			}
		}
		return destination;
	}
}