#pragma once

#include "int_range.h"
#include "in_range.h"
#include "push_back_unique.h"

#include <array>
#include <vector>
#include <optional>
#include <algorithm>
#include <map>
#include <cassert>
#include <functional>

namespace utils
{

	template <std::size_t DIM>
	class conway_simulation
	{
	public:
		using CoordType = int;
		using Coord = std::array<CoordType, DIM>;
		using PointData = std::vector<Coord>;
		using Limit = std::optional<CoordType>;
		using Bounds = std::pair<Limit, Limit>;
		using Space = std::array<Bounds, DIM>;
	private:
		mutable PointData m_active_points;
		Space m_space;
		std::pair<std::size_t, std::size_t> turn_on_range;
		std::pair<std::size_t, std::size_t> stay_on_range;
		mutable std::map<Coord, PointData> m_neighbour_cache;
		mutable std::map<Coord, std::size_t> m_num_active_neighbours_cache;
		mutable bool m_sorted = false;

		static bool verify_bounds(const Bounds& bound, CoordType val)
		{
			if (bound.first.has_value() && bound.first.value() > val)
			{
				return false;
			}
			if (bound.second.has_value() && bound.second.value() < val)
			{
				return false;
			}
			return true;
		}

		static void plus_equal(Coord& left, const Coord& right)
		{
			for (auto i : int_range(DIM))
			{
				left[i] += right[i];
			}
		}

		static Coord plus(const Coord& a, const Coord& b)
		{
			Coord result = a;
			plus_equal(result, b);
			return result;
		}

		void create_options(PointData& result, const Coord& center, Coord& base, std::size_t index) const
		{
			if (index == DIM)
			{
				if (std::all_of(begin(base), end(base), [](CoordType v) {return v == 0; }))
				{
					return;
				}
				const Coord new_coord = plus(center, base);
				result.push_back(new_coord);
				return;
			}

			for (CoordType i : int_range(-1, 2))
			{
				const CoordType dim_value = center[index] + i;
				const Bounds& bounds = m_space[index];
				if (verify_bounds(bounds, dim_value))
				{
					base[index] = i;
					create_options(result, center, base, index + 1);
				}
			}
		}

		PointData calculate_neighbours(const Coord& coord) const
		{
			std::size_t new_size = 1;
			for (auto i : int_range(DIM))
			{
				new_size *= 3;
			}
			PointData result;
			result.reserve(new_size - 1);
			Coord base;
			create_options(result, coord, base, 0);
			return result;
		}

		PointData get_neighbours(const Coord& coord) const
		{
			const auto find_result = m_neighbour_cache.find(coord);
			if (find_result != end(m_neighbour_cache))
			{
				return find_result->second;
			}
			PointData result = calculate_neighbours(coord);
			m_neighbour_cache[coord] = result;
			return result;
		}

		PointData get_all_relevant_points() const
		{
			PointData result;
			for (const Coord& coord : m_active_points)
			{
				const auto neighbours = get_neighbours(coord);
				result.reserve((neighbours.size() + 1) * m_active_points.size());
				result.push_back(coord);
				std::copy(begin(neighbours), end(neighbours), std::back_inserter(result));
			}
			std::sort(begin(result), end(result));
			result.erase(std::unique(begin(result), end(result)), end(result));
			return result;
		}

		std::size_t calculate_num_active_neighbours(const Coord& coord) const
		{
			const auto neighbours = get_neighbours(coord);
			return std::count_if(begin(neighbours), end(neighbours),
				[this](const Coord& c) {return is_active(c); });
		}

		std::size_t get_num_active_neighbours(const Coord& coord) const
		{
			const auto find_result = m_num_active_neighbours_cache.find(coord);
			if (find_result != end(m_num_active_neighbours_cache))
			{
				return find_result->second;
			}
			else
			{
				const std::size_t result = calculate_num_active_neighbours(coord);
				m_num_active_neighbours_cache[coord] = result;
				return result;
			}
		}

		bool should_activate_on_tick(const Coord& coord) const noexcept
		{
			const auto num_active_neighbours = get_num_active_neighbours(coord);
			const auto& active_range = is_active(coord) ? stay_on_range : turn_on_range;
			return in_range(num_active_neighbours, active_range.first, active_range.second);
		}

	public:
		conway_simulation(PointData active_points,
			std::size_t turn_on_min, std::size_t turn_on_max,
			std::size_t stay_on_min, std::size_t stay_on_max,
			Space bounds = Space{})
			: m_active_points{ std::move(active_points) }
			, turn_on_range{ std::make_pair(turn_on_min,turn_on_max) }
			, stay_on_range{ std::make_pair(stay_on_min,stay_on_max) }
			, m_space{ bounds }
		{}
		conway_simulation(const conway_simulation&) = default;
		conway_simulation(conway_simulation&&) = default;
		conway_simulation& operator=(const conway_simulation&) = default;
		conway_simulation& operator=(conway_simulation&&) = default;

		void tick()
		{
			PointData new_points;
			const PointData relevant_points = get_all_relevant_points();
			std::copy_if(begin(relevant_points), end(relevant_points), std::inserter(new_points, begin(new_points)),
				[this](const Coord& point) {return should_activate_on_tick(point); });
			m_active_points = std::move(new_points);
			m_num_active_neighbours_cache.clear();
			m_sorted = false;
		}

		bool is_active(const Coord& point) const noexcept
		{
			if (!m_sorted)
			{
				std::sort(begin(m_active_points), end(m_active_points));
				m_sorted = true;
			}
			return std::binary_search(begin(m_active_points), end(m_active_points), point);
		}

		const PointData& get_active_points() const noexcept
		{
			return m_active_points;
		}
	};

}