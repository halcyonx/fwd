#include <cassert>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <vector>

template <typename F, typename... Args>
void for_each_arg(F&& fn, Args&&... args)
{
	[](...){}( (fn(std::forward<Args>(args)), 0)... );
}

template <typename F, typename... Args>
void for_args(F&& fn, Args&&... args)
{
	return (void) std::initializer_list<int>
	{
		( 
			fn( std::forward<Args>(args) )
			, 0 
		)...
	};
}

template <typename...TArgs>
auto make_vector(TArgs... args)
{
	using VectorItemT = std::common_type_t<TArgs...>;

	std::vector<VectorItemT> vec;

	vec.reserve(sizeof...(TArgs));

	for_args
	(
		[ &vec ](auto && a)
		{
			vec.emplace_back( std::forward<decltype(a)>(a) );
		}
		, std::forward<TArgs>(args)...
	);

	return vec;
}

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3802.pdf
template <typename F, typename Tuple, size_t... I>
decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
	return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}
template <typename F, typename Tuple>
decltype(auto) apply(F&& f, Tuple&& t) 
{
	using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
	return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
}

template <typename Fn, typename Tuple>
void 
for_tuple(Fn&& fn, Tuple&& tuple)
{
	apply
	(
		[ &fn ](auto&&... xs)
		{
			for_args
			(
				fn,
				std::forward<decltype(xs)>(xs)...
			);
		},
		std::forward<Tuple>(tuple)
	);
}

int main(int argc, char const *argv[])
{
	for_args
	(
		[](const auto & a) { std::cout << a << " "; }
		, 1
		, 2
		, 3
		, 3.1415926535f
		, "Hello, world!"
	);

	auto vec = make_vector(1, 2, 3, 4, 5);

	static_assert(std::is_same<decltype(vec), std::vector<int>>(), "");

	for (auto i : vec)
	{
		std::cout << i << " ";
	}

	std::cout << "\n";

	for_tuple
	(
		[](const auto& a) { std::cout << a << " "; },
		std::make_tuple(1, 2, 3.1415926535f, "Test string")
	);

	std::cout << "\n";

	return 0;
}