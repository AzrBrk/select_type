#pragma once
#include<iostream>
#include"flex_string.hpp"

namespace exp_print
{
	using namespace flex_string;
	using namespace flex_string::flex_string_space;
	
	template<class T> struct meta_print
	{
		void operator()() { std::cout << typeid(T).name(); }
	};

	template<template<size_t> class ival_type, size_t I> struct meta_print<ival_type<I>>
	{
		void operator()() { std::cout << '<' << I << '>'; }
	};

	template<template<char> class cval_type, char C> struct meta_print<cval_type<C>>
	{
		void operator()() { std::cout <<  C; }
	};
	template<template<class ...> class TL, class ...LS> struct meta_print<TL<LS...>>
	{
		template<template<class ...> class MTL, class ...T> 
		void print_impl(MTL<T...> const& l)
		{
			std::cout << "O.o<";
			((meta_print<T>{}.operator()()), ...);
			std::cout << ">o.O";
		}
		void operator()()
		{
			using exp_TL = typename to_exp_list<TL<LS...>>::type;
			using add_comma = exp_TL::template to<delim_list>;
			using add_comma_type = add_comma::template apply<exp_char<','>>;
			print_impl(add_comma_type{});
		}
	};
	template<class looper> constexpr void track_looper()
	{
		if constexpr (!meta_invoke<looper>::_continue_) { return; }
		if constexpr (meta_invoke<looper>::_continue_) {
			using current_mo = typename meta_invoke<looper>::result_stage_o::type;
			std::cout << "current_mo_type: ";
			meta_print<current_mo>{}();
			std::cout << std::endl;
			track_looper<typename meta_invoke<looper>::next_stage>();
		}

	}
	template<class looper> constexpr void track_stream()
	{
		if constexpr (!meta_invoke<looper>::_continue_) { return; }
		if constexpr (meta_invoke<looper>::_continue_) {
			using current_mo = typename meta_invoke<looper>::result_stage_o::type::to::type;
			std::cout << "current_opstream_type: ";
			meta_print<current_mo>{}();
			std::cout << std::endl;
			track_stream<typename meta_invoke<looper>::next_stage>();
		}

	}
}