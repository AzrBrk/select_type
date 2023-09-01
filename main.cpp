#include"join_list.h"
#include"exp_function_series.hpp"

using namespace exp_function_series;


constexpr auto _shift = true;

template<size_t N, class NODE>
typename select_type<N, typename NODE::element_type_list>::type get_from_node(NODE& node, exp_index<N> idx)
{
	typename select_type<N, typename NODE::element_type_list>::type val;
	exp_iterator it{ node };
	return fetch_value(val, it);
}

void print(auto&& x, bool shift = false)
{
	std::cout << x;
	if (shift) std::cout << std::endl;
}

void print_node(auto& node)
{
	exp_iterator iter{ node };
	for (auto i = 0; i < iter.size(); ++i)
	{
		print(iter[i]);
		print(' ');
	}
}
void print_type(auto&& x)
{
	print(typeid(x).name(), _shift);
}
int foo(int a) { return a; };
double foo2(int a, double b) { return a + b; };
int foo3(int a, int b, int c) { return a + b + c; };



auto main() -> int
{
	auto f_s = link_f(foo, foo2, foo3);

	exp_iterator f_s_iter{ *f_s };

	auto f_ar_link = link_arguments(f_s);

	print_type(f_ar_link);

	exp_iterator f_ar_iter{ f_ar_link };

	for (auto i = 0; i < f_ar_iter.size(); ++i)
	{
		f_ar_iter[i] = i*2;
	}

	f_ar_iter[2] = 3.78;

	auto invoke_f = [](auto& value) { print(value.apply_func(),_shift); };

	loop_with(f_s.func_nodes, invoke_f);
}