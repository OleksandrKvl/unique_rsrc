#ifndef UNIQUE_RSRC_H
#define UNIQUE_RSRC_H

#include <memory>
#include <type_traits>
#include <functional>
#include <ostream>
#include <utility>

namespace ursrc
{
	template<typename RsrcTraits>
	class unique_rsrc
	{
	public:
		using traits_type = RsrcTraits;
		using value_type = typename traits_type::value_type;

		unique_rsrc(const unique_rsrc&) = delete;
		unique_rsrc& operator=(const unique_rsrc&) = delete;

		~unique_rsrc()
		{
			if (*this)
			{
				traits_type::destruct(resource);
			}
		}

		unique_rsrc() = default;

		explicit unique_rsrc(const value_type resource) noexcept
			: resource{ resource }
		{
		}

		unique_rsrc(unique_rsrc && rhs) noexcept
			: resource{ rhs.release() }
		{
		}

		unique_rsrc & operator=(unique_rsrc && rhs) noexcept
		{
			if (this != std::addressof(rhs))
			{
				reset(rhs.release());
			}
			return *this;
		}

		explicit operator bool() const noexcept
		{
			return resource != traits_type::null_value;
		}

		value_type get() const noexcept
		{
			return resource;
		}

		void reset(const value_type resource = traits_type::null_value) noexcept
		{
			const auto old = this->resource;
			this->resource = resource;
			if (old != traits_type::null_value)
			{
				traits_type::destruct(old);
			}
		}

		value_type release() noexcept
		{
			const auto tmp{ resource };
			resource = traits_type::null_value;
			return tmp;
		}

		value_type operator*() const noexcept
		{
			return get();
		}

		void swap(unique_rsrc& other) noexcept
		{
			std::swap(this->resource, other.resource);
		}

	private:
		value_type resource{ traits_type::null_value };
	};

	template<typename T>
	bool operator==(
		const unique_rsrc<T> & lhs, const unique_rsrc<T> & rhs) noexcept
	{
		return lhs.get() == rhs.get();
	}
	template<typename T>
	bool operator!=(
		const unique_rsrc<T> & lhs, const unique_rsrc<T> & rhs) noexcept
	{
		return !(lhs == rhs);
	}

	template<typename T>
	bool operator<(
		const unique_rsrc<T> & lhs, const unique_rsrc<T> & rhs) noexcept
	{
		return std::less<typename T::value_type>{}(lhs.get(), rhs.get());
	}

	template<typename T>
	bool operator<=(
		const unique_rsrc<T> & lhs, const unique_rsrc<T> & rhs) noexcept
	{
		return !(rhs < lhs);
	}

	template<typename T>
	bool operator>(
		const unique_rsrc<T> & lhs, const unique_rsrc<T> & rhs) noexcept
	{
		return rhs < lhs;
	}

	template<typename T>
	bool operator>=(
		const unique_rsrc<T> & lhs, const unique_rsrc<T> & rhs) noexcept
	{
		return !(lhs < rhs);
	}

	template <typename CharT, typename Traits, typename T>
	std::basic_ostream<CharT, Traits> & operator<<(
		std::basic_ostream<CharT, Traits> & os, const unique_rsrc<T> & h)
	{
		os << h.get();
		return os;
	}

	template<typename T, typename... Ts>
	unique_rsrc<T> make_unique_rsrc(Ts&&... args)
		noexcept(noexcept(T::construct(std::forward<Ts>(args)...)))
	{
		return unique_rsrc<T>{ T::construct(std::forward<Ts>(args)...) };
	}
}

namespace std 
{
	template<typename T>
	struct hash<ursrc::unique_rsrc<T>>
	{
		using argument_type = ursrc::unique_rsrc<T>;
		using result_type = std::size_t;
		result_type operator()(const argument_type & resource) const
		{
			return std::hash<typename T::value_type>{}(resource.get());
		}
	};
}

#endif //UNIQUE_RSRC_H