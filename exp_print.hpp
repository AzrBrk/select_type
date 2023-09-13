#pragma once
#include<iostream>
#include"select_type.hpp"
namespace exp_print
{
	void print(auto&& x);
	void print(auto&&...x);
	void print_as_line(auto&&...x);
	void print_type(auto&& x);
	void print_tuple(auto& tp, char delim =',');

	void print(auto&& x)
	{
		std::cout << x;
	}
	void print(auto&&...x)
	{
		(exp_print::print(x), ...);
	}
	void print_as_line(auto&& ...x)
	{
		exp_print::print(x...), exp_print::print('\n');
	}
	void print_type(auto&& x)
	{
		exp_print::print(typeid(x).name());
		exp_print::print('\n');
	}

	void print_tuple(auto& tp, char delim)
	{
		tuple_iterator it{ tp };
		if (it.iterator().size()==0) { print(it.iterator()); return; }
		for (auto i = 0; i < it.iterator().size() -1; ++i) print(it.iterator()[i], delim);
		print(it.iterator()[++(it.iterator().exp_index)]);
	}

}