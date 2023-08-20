#pragma once
#include"exp_function_binder.h"

namespace exp_bind
{
	template<class T, class TL>
	struct add_to_front {};

	template<class T, template<class...> class TL, class ...L>
	struct add_to_front<T, TL<L...>>
	{
		using type = TL<T, L...>;
	};

	template<class TL>
	struct typelist_to_fn
	{};

	template<template<class...> class TL, class R, class ...L>
	struct typelist_to_fn<TL<R, L...>>
	{
		using type = R(L...);
		using Ret_Type = R;
	};


	template<class _node, class T, class ...L>
	_node join_list_impl(T const& fv, L...l)
	{
		_node _n(fv);
		(::push_back(_n, l, []<class Ty, class T>(T const& val) { return std::make_shared<Ty>(val); }), ...);
		return _n;
	}

	template<class R, template<class...> class TL, class ...L>
	typename decltype(join_list_impl<R, L...>) get_join_func(TL<R, L...> const& tl)
	{
		return join_list_impl<R, L...>;
	}

	template<class _node1, class _node2>
	auto join_list(_node1& _n1, _node2& _n2)
	{
		using join_list_type_impl = typename exp_join<
			typename _node1::element_type_list,
			typename _node2::element_type_list
		>::type;

		using node_type = element_node<0, join_list_type_impl, std::shared_ptr>;

		using join_list_type = typename add_to_front <node_type, join_list_type_impl>::type;

		auto  pjoinf = get_join_func(join_list_type());

		exp_bind::exp_function_binder<typename typelist_to_fn<join_list_type>::type> efb{ pjoinf };

		auto bind_func = [&efb](auto value) { efb.bind(value); };

		loop_with(_n1, bind_func);
		loop_with(_n2, bind_func);
		return efb.apply_func();
	}
}
