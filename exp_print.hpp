#pragma once
#include<iostream>
#include"select_type.hpp"
#include"exp_function_series.hpp"
#define TRACK_ARGUMENT_TEXT "arguments: "
#define TRACK_RETURN_TEXT "returns:   "
namespace exp_print
{
	void print(auto&& x);
	void print(auto&&...x);
	void print_as_line(auto&&...x);
	void print_type(auto&& x);
	void print_tuple(auto& tp, char delim =',');

	void _shift()
	{
		std::cout << '\n';
	}
	template<class output>
	void print(output&& x)
	{
		if constexpr (requires(output op, std::ostream & os) { os << op; })
			std::cout << x;
		else
			std::cout << typeid(x).name();
	}
	void print(auto&&...x)
	{
		(exp_print::print(x), ...);
	}
	void print_as_line(auto&& ...x)
	{
		exp_print::print(x...), _shift();
	}
	void print_type(auto&& x)
	{
		exp_print::print(typeid(x).name());
		_shift();
	}

	void print_tuple(auto& tp, char delim)
	{
		tuple_iterator it{ tp };
		if (it.iterator().size() <= 1) { 
			print(it.iterator()); 
			return; 
		}
		for (auto i = 0; i < it.iterator().size() -1; ++i)
			print(it.iterator()[i], delim);
		print(it.iterator()[++(it.iterator().exp_index)]);
	}

	//use a track_series to track a function series
	//also an example on usage of exp_function_series::series
	template<class _tracked_series_> void track_series(_tracked_series_& trackedSeries, size_t idx = 0, 
		const char * ar_text = TRACK_ARGUMENT_TEXT, 
		const char * re_text = TRACK_RETURN_TEXT)
	{
		//create reference to function_series return stack as tuple;
		auto& ret_stack = trackedSeries[idx]._return();
		//create a print series for an elegant loop
		auto print_series = exp_function_series::link_f(
			[&trackedSeries, &ar_text]() {
				loop_with(*trackedSeries, [&ar_text](auto& efb)
					{
						exp_print::print(ar_text);
						print_tuple(efb.args_stack);
						_shift(); 
					});
			},
			[&trackedSeries]() {
				trackedSeries.execute(); 
				trackedSeries.bind_last_return(); 
			},
			[&ret_stack, &re_text]() {
				exp_print::print(re_text);
				print_tuple(ret_stack);
				_shift(); 
			}
		);
		for (auto i = 0; i < trackedSeries.size(); ++i)
		{
			print_series[0]._continue();
		}
	}

}