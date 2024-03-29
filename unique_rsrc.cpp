#include <iostream>
#include <type_traits>
#include <cassert>

#include "unique_rsrc.h"

std::size_t g_destruct_called_times = 0;
std::size_t g_construct_called_times = 0;

struct handle_traits
{
	using value_type = int;
	static const value_type null_value{ -1 };

	static value_type construct(int i, bool /*b*/)
	{
		g_construct_called_times++;
		return i;
	}

	static void destruct(value_type /*value*/) noexcept
	{
		g_destruct_called_times++;
	}
};

using smart_handle = ursrc::unique_rsrc<handle_traits>;

int main()
{
	static_assert(
		std::is_nothrow_default_constructible<smart_handle>::value == true,
		"default constructible");
	static_assert(
		std::is_nothrow_constructible<
		smart_handle, handle_traits::value_type>::value == true,
		"constructible from value_type");
	static_assert(
		std::is_copy_constructible<smart_handle>::value == false,
		"not copy constructible");
	static_assert(
		std::is_copy_assignable<smart_handle>::value == false,
		"not copy assignable");
	static_assert(
		std::is_nothrow_move_constructible<smart_handle>::value == true,
		"move constructible");
	static_assert(
		std::is_nothrow_move_assignable<smart_handle>::value == true,
		"move assignable");

	{
		smart_handle handle{ 2 };
		assert(handle.get() == 2);

		assert(bool{ handle } == true);

		assert(*handle == 2);

		//destruct 1
		handle.reset(3);
		assert(*handle == 3);

		//destruct 2
		handle.reset();
		assert(bool{ handle } == false);
		assert(*handle == handle_traits::null_value);

		handle.reset(4);
		const auto released = handle.release();
		assert(released == 4);
		assert(*handle == handle_traits::null_value);
		assert(g_destruct_called_times == 2);
	}

	{
		smart_handle a{ 2 };
		smart_handle b{ 3 };
		smart_handle c{ 2 };

		assert(a < b);
		assert(b > a);
		assert(a <= b);
		assert(b >= a);
		assert(a == c);
	}

	{
		smart_handle handle{ 2 };
		std::hash<smart_handle>{}(handle);

		std::cout << handle;
	}

	{
		smart_handle a{ 2 };
		smart_handle b{ 3 };
		a.swap(b);
		
		assert(*a == 3);
		assert(*b == 2);
	}

	{
		auto handle = ursrc::make_unique_rsrc<handle_traits>(1, false);
		assert(g_construct_called_times == 1);
	}

	return 0;
}