unique_rsrc
===

This is analog of `std::unique_ptr` for resources that are represented
by values, not pointers, for example Windows `HANDLE` is a value, and it
has a different `null_value`, depending on actual resource it can be
INVALID_HANDLE_VALUE or just 0.   

### Example
```c++
struct handle_traits
{
	using value_type = int;
	static const value_type null_value{ -1 };

	static value_type construct(int i, bool /*b*/)
	{
		return i;
	}

	static void destruct(value_type /*value*/) noexcept
	{
	}
};
using smart_handle = ursrc::unique_rsrc<handle_traits>;

smart_handle handle{ 2 };
assert(handle.get() == 2);
```
`construct` is required only if `make_unique_rsrc` is used.

### Interface
Interface is mostly the same as `std::unique_ptr` with minor changes.
There's no operator->(), operator[], get_deleter(), no constructor/reset
from unique_rsrc with convertible type. `make_unique_rsrc` is playing role 
of `std::make_unique`.
#### Member functions
```c++
~unique_rsrc();
unique_rsrc();
explicit unique_rsrc(const value_type resource) noexcept;
unique_rsrc(unique_rsrc && rhs) noexcept;
unique_rsrc & operator=(unique_rsrc && rhs) noexcept;

explicit operator bool() const noexcept;

value_type get() const noexcept;
void reset(value_type resource = traits_type::null_value) noexcept;
value_type release() noexcept;
value_type operator*() const noexcept;
void swap(unique_rsrc& other) noexcept;
```
#### Non-member functions
```c++
unique_rsrc<traits> make_unique_rsrc<traits>(args...);

operator==
operator!=
operator<
operator<=
operator>
operator>=

operator<<(std::basic_ostream<CharT, Traits> & os, const unique_rsrc<T> & h);

std::hash<unique_rsrc<T>>;
```