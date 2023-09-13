#pragma once
#include<utility>
namespace function_impl
{
	template<class TL, class F>
	struct meta_function {};

	template<template<class...> class TL, class F, class ...L>
	struct meta_function<TL<L...>, F>
	{
		F func;
		meta_function(TL<L...> const& tl, F const& f) :func(f) {}
		auto operator()(L...l)
		{
			return func.template operator()<L...>(std::forward<L>(l)...);
		}
	};

	template<template<class...> class TL, class F, class ...L>
	meta_function(TL<L...>const& tl, F const& f) -> meta_function<TL<L...>, F>;

	template<class TL, class F>
	auto realize_meta(F const & f)
	{
		return meta_function{ TL(), f };
	}
}