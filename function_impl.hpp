#pragma once
#include<utility>
#include"meta_selectable_list.hpp"

namespace function_impl
{
	using namespace meta_typelist;

	template<class TL, class F>
	struct meta_function {};

	template<template<class...> class TL, class F, class ...L>
	struct meta_function<TL<L...>, F>
	{
		F func;
		meta_function(TL<L...> const& tl, F const& f) :func(f) {}
		auto operator()(L...l)
		{
			return func.template operator() < L... > (std::forward<L>(l)...);
		}
	};

	template<template<class...> class TL, class F, class ...L>
	meta_function(TL<L...>const& tl, F const& f) -> meta_function<TL<L...>, F>;

	template<class TL, class F>
	auto realize_meta(F const& f)
	{
		return meta_function{ TL(), f };
	}

	template<class F>
	struct decl_lamb {};

	template<class R, class T, class ...Args>
	struct decl_lamb<R(T::*)(Args...)const>
	{
		using type = R(T::*)(Args...)const;
		using non_mem_const = R(Args...);
		using arguments = selectable_list<Args...>;
	};
	template<auto lamb>
	struct lamb_info
	{
		using lamb_type = decltype(lamb);
		using invoke_type = decl_lamb<decltype(&decltype(lamb)::operator())>::type;
		using fn_type = decl_lamb<decltype(&decltype(lamb)::operator())>::non_mem_const;
		using args_type = decl_lamb<decltype(&decltype(lamb)::operator())>::arguments;
		static const size_t args_counts = size_of_type_list<args_type>::value;
		std::vector<const char*> operator*()
		{
			return { typeid(lamb_type).name(), typeid(invoke_type).name(), typeid(fn_type).name(),typeid(args_type).name() };
		}
	};

	template<auto fn_ptr>
	using fn_t = std::remove_pointer_t<decltype(fn_ptr)>;

}