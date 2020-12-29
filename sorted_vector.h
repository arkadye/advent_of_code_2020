#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>

namespace utils
{

	template <typename T>
	class sorted_vector
	{
	public:
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;
		using Pred_t = std::function<bool(const T&, const T&)>;
		using value_type = T;
	private:
		mutable std::vector<T> m_data;
		Pred_t m_compare;
		mutable bool m_sorted;
	public:
		sorted_vector(Pred_t compare = std::less<T>())
			: m_data()
			, m_compare(compare)
			, m_sorted(true)
		{}
		template <typename InputIt>
		sorted_vector(InputIt start, InputIt finish, Pred_t compare = std::less<T>())
			: m_data(start, finish)
			, m_compare(compare)
			, m_sorted(false)
		{}

		sorted_vector(std::initializer_list<T> ilist) : sorted_vector(ilist.begin(), ilist.end()) {}

		sorted_vector(const sorted_vector&) = default;
		sorted_vector(sorted_vector&&) = default;
		sorted_vector& operator=(const sorted_vector&) = default;
		sorted_vector& operator=(sorted_vector&&) = default;

		void sort() const
		{
			if (!m_sorted)
			{
				std::sort(m_data.begin(), m_data.end(), m_compare);
				m_sorted = true;
			}
		}

		void clear()
		{
			m_data.clear();
			m_sorted = true;
		}

		bool empty() const
		{
			return m_data.empty();
		}

		std::size_t size() const
		{
			return m_data.size();
		}

		auto lower_bound(const T& value)
		{
			sort();
			return std::lower_bound(m_data.begin(), m_data.end(), value, m_compare);
		}

		auto upper_bound(const T& value)
		{
			sort();
			return std::upper_bound(m_data.begin(), m_data.end(), value, m_compare);
		}

		auto lower_bound(const T& value) const
		{
			sort();
			return std::lower_bound(m_data.begin(), m_data.end(), value, m_compare);
		}

		auto upper_bound(const T& value) const
		{
			sort();
			return std::upper_bound(m_data.begin(), m_data.end(), value, m_compare);
		}

		auto find(const T& value)
		{
			const auto result = lower_bound(value);
			return (result != end() && *result == value) ? result : end();
		}

		auto find(const T& value) const
		{
			const auto result = lower_bound(value);
			return (result != end() && *result == value) ? result : end();
		}

		const T& front() const
		{
			sort();
			return m_data.front();
		}

		T& front()
		{
			sort();
			return m_data.front();
		}

		const T& back() const
		{
			sort();
			return m_data.back();
		}

		T& back()
		{
			sort();
			return m_data.back();
		}

		bool contains(const T& value)
		{
			return find(value) != end();
		}

		void erase(const_iterator pos)
		{
			assert(m_sorted);
			m_data.erase(pos);
		}

		void erase(const_iterator start, const_iterator finish)
		{
			assert(m_sorted);
			m_data.erase(start, finish);
		}

		void erase_fast(iterator pos)
		{
			assert(m_sorted);
			if (pos != (end() - 1))
			{
				*pos = m_data.back();
			}
			m_data.pop_back();
			m_sorted = false;
		}

		void erase(const T& val)
		{
			const auto pos = find(val);
			if (pos != end())
			{
				erase(pos);
			}
		}

		void erase_fast(const T& val)
		{
			const auto pos = find(val);
			if (pos != end())
			{
				erase_fast(pos);
			}
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last)
		{
			m_data.insert(end(m_data), first, last);
			m_sorted = false;
			break;
		}

		// Implies keep_sorted = true. Tries to insert just before hint.
		iterator insert(const_iterator hint, T&& value)
		{
			if (m_data.empty())
			{
				m_data.push_back(std::forward<T>(value));
				m_sorted = true;
				return m_data.end() - 1;
			}

			if (!m_sorted)
			{
				return insert(std::forward<T>(value));
			}

			const bool check_after = (hint == m_data.cend() || !m_compare(*hint, value));
			if (check_after)
			{
				const bool check_before = (hint == m_data.cbegin() || !m_compare(value, *(hint - 1)));
				if (check_before)
				{
					return m_data.insert(hint, std::forward<T>(value));
				}
			}
			return insert(std::forward<T>(value));
		}

		// Implies keep_sorted = true. Tries to insert just before hint.
		iterator insert(const_iterator hint, const T& value)
		{
			if (m_data.empty())
			{
				m_data.push_back(value);
				m_sorted = true;
				return m_data.end() - 1;
			}

			if (!m_sorted) // Hint is meaningless here anyway.
			{
				return insert(value);
			}

			const bool check_after = (hint == m_data.cend() || !m_compare(*hint, value));
			if (check_after)
			{
				const bool check_before = (hint == m_data.cbegin() || !m_compare(value, *(hint - 1)));
				if (check_before)
				{
					return m_data.insert(hint, value);
				}
			}
			return insert(value);
		}

		iterator insert(T&& value)
		{
			m_sorted = m_data.empty() || (m_sorted && !m_compare(value, m_data.back()));
			m_data.push_back(std::forward<T>(value));
			return m_data.end() - 1;
		}

		iterator insert(const T& value)
		{
			m_sorted = m_data.empty() || (m_sorted && !m_compare(value, m_data.back()));
			m_data.push_back(value);
			return m_data.end() - 1;
		}

		// For std::back_inserter compatibility
		void push_back(T&& value)
		{
			insert(std::forward<T>(value));
		}

		void push_back(const T& value)
		{
			insert(value);
		}

		T& operator[](std::size_t index)
		{
			assert(index < m_data.size());
			sort();
			return m_data[index];
		}
		const T& operator[](std::size_t index) const
		{
			assert(index < m_data.size());
			sort();
			return m_data[index];
		}
		auto begin() { sort(); return m_data.begin(); }
		auto begin() const { sort(); return m_data.cbegin(); }
		auto end() { sort(); return m_data.end(); }
		auto end() const { sort(); return m_data.cend(); }
		auto cbegin() const { sort(); return m_data.cbegin(); }
		auto cend() const { sort(); return m_data.cend(); }
		auto rbegin() { sort(); return m_data.rbegin(); }
		auto rbegin() const { sort(); return m_data.crbegin(); }
		auto rend() { sort(); return m_data.rend(); }
		auto rend() const { sort(); return m_data.crend(); }
		auto crbegin() const { sort(); return m_data.crbegin(); }
		auto crend() const { sort(); return m_data.crend(); }
	};
}

template <typename T>
inline auto begin(utils::sorted_vector<T>& sv) { return sv.begin(); }

template <typename T>
inline auto begin(const utils::sorted_vector<T>& sv) { return sv.begin(); }

template <typename T>
inline auto end(utils::sorted_vector<T>& sv) { return sv.end(); }

template <typename T>
inline auto end(const utils::sorted_vector<T>& sv) { return sv.end(); }

template <typename T>
inline auto rbegin(utils::sorted_vector<T>& sv) { return sv.rbegin(); }

template <typename T>
inline auto rbegin(const utils::sorted_vector<T>& sv) { return sv.rbegin(); }

template <typename T>
inline auto rend(utils::sorted_vector<T>& sv) { return sv.rend(); }

template <typename T>
inline auto rend(const utils::sorted_vector<T>& sv) { return sv.rend(); }

template <typename T>
inline auto cbegin(const utils::sorted_vector<T>& sv) { return sv.cbegin(); }

template <typename T>
inline auto cend(const utils::sorted_vector<T>& sv) { return sv.cend(); }

template <typename T>
inline auto crbegin(const utils::sorted_vector<T>& sv) { return sv.crbegin(); }

template <typename T>
inline auto crend(const utils::sorted_vector<T>& sv) { return sv.crend(); }