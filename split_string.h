#pragma once

#include <string_view>
#include <vector>

namespace utils
{

	inline std::vector<std::string_view> split_string(std::string_view str, char delim)
	{
		std::vector<std::string_view> result;
		while (true)
		{
			const std::size_t delim_loc = str.find(delim);
			if (delim_loc != std::string_view::npos)
			{
				result.push_back(str.substr(0, delim_loc));
				str.remove_prefix(delim_loc + 1); // +1 to remove delimiter as well.
			}
			else
			{
				result.push_back(str);
				return result;
			}
		}
		return result;
	}
}