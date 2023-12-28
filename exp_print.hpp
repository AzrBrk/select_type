#pragma once
#include<iostream>
#include"flex_string.hpp"

namespace exp_print
{
	using namespace flex_string;
	using namespace flex_string::flex_string_space;
	
	template<class T> struct meta_print
	{
		std::ostream& operator()(std::ostream& os = std::cout) { os << typeid(T).name(); return os; }
	};

	template<template<size_t> class ival_type, size_t I> struct meta_print<ival_type<I>>
	{
		std::ostream& operator()(std::ostream& os = std::cout) { os << '<' << I << '>'; return os; }
	};

	template<template<size_t, class> class tagged, size_t I, class T> struct meta_print<tagged<I, T>>
	{
		std::ostream& operator()(std::ostream& os = std::cout)
		{
			os << "template<" << I << ",";
			meta_print<T>{}(os) << ">";
			return os;
		}
	};
	template<class obj, class F> struct meta_print<meta_object<obj, F>>
	{
		std::ostream& operator()(std::ostream& os = std::cout)
		{
			os << "MO{\n object = ";
			meta_print<obj>{}(os) << "\n function = ";
			meta_print<F>{}(os) << "\n}";
			return os;
		}
	};
	template<class TL> struct meta_print<meta_istream<TL>>
	{
		std::ostream& operator()(std::ostream & os= std::cout)
		{
			os << "SOURCE{";
			meta_print<TL>{}(os) << "}";
			return os;
		}
	};
	template<class TL> struct meta_print<meta_ostream<TL>>
	{
		std::ostream& operator()(std::ostream& os = std::cout)
		{
			os << "DEST{";
			meta_print<TL>{}(os) << "}";
			return os;
		}
	};

	template<class TL, class Fn> struct meta_print<meta_appendable_filter_o<TL, Fn>>
	{
		std::ostream& operator()(std::ostream& os = std::cout)
		{
			os << "FILTER{";
			meta_print<TL>{}(os) << ":";
			meta_print<Fn>{}(os) << "}";
			return os;
		}
	};

	template<size_t I, class obj, class F, class B> struct meta_print<meta_timer_object<I, obj, F, B>>
	{
		std::ostream& operator()(std::ostream& os = std::cout)
		{
			os << "TIMER<" << I << ">:(break_condition:";
			meta_print<B>{}(os) << ")\n";
			meta_print<meta_object<obj, F>>{}(os);
			return os;
		}
	};

	template<template<char> class cval_type, char C> struct meta_print<cval_type<C>>
	{
		std::ostream& operator()(std::ostream& os = std::cout) { os << C; return os; }
	};
	template<template<class ...> class TL, class ...LS> struct meta_print<TL<LS...>>
	{
		template<template<class ...> class MTL, class ...T> 
		std::ostream& print_impl(MTL<T...> const& l, std::ostream& os = std::cout)
		{
			os << "template<";
			((meta_print<T>{}.operator()(os)), ...);
			os << ">";
			return os;
		}
		std::ostream& operator()(std::ostream& os = std::cout)
		{
			using exp_TL = typename to_exp_list<TL<LS...>>::type;
			using add_comma = exp_TL::template to<delim_list>;
			using add_comma_type = add_comma::template apply<exp_char<','>>;
			return print_impl(add_comma_type{}, os);
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