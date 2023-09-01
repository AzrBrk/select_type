#pragma once
#include"join_list.h"
namespace exp_function_series
{
	using namespace exp_bind;

	template<class EFB>
	using _arg_type_list = typename EFB::argument_list_type;

	template<class TL>
	using wrap_for_each_TL = typename exp_apply<TL, ref_wrapper>::type;

	template<class TL>
	using rename_to_tuple = typename exp_rename<TL, std::tuple>::type;

	template<class ...EFBs>
	struct series_types
	{
		using arg_list = typename exp_apply<exp_list<EFBs...>, _arg_type_list>::type;
		using tuples_type = typename exp_apply<arg_list, rename_to_tuple>::type;
		using wrapped_arg_list = typename exp_apply<arg_list, wrap_for_each_TL>::type;
		using arg_node_type = typename exp_rename<wrapped_arg_list, exp_shared_node>::type;
	};

	template<class ...EFBS>
	struct series
	{
		exp_shared_node<EFBS...> func_nodes;
		exp_iterator< exp_shared_node<EFBS...>> func_iter;
		series(exp_shared_node<EFBS...> const& efbs) : func_nodes(efbs), func_iter(func_nodes) {}
		auto& operator*() { return func_nodes; }
	};

	template<class ...EFBS>
	series<EFBS...> link_funcs(EFBS&& ...efbs)
	{
		series<EFBS...> efb_series{ make_element_node(shared_constructor(),efbs...) };

		return efb_series;
	}
	template<class ...EFBS>
	auto link_arguments(series<EFBS...>& efb_series)
	{
		auto func_impl = []<class ...L>(L*...l) { return link_a_lot_tuples((*l)...); };
		auto make_func_impl = [&func_impl]<template<class...> class typelist, class ...L>(typelist<L...> tl)
		{
			return [&func_impl](L*...l) { return func_impl.template operator()(l...); };
		};

		using tl_type = typename series_types<EFBS...>::tuples_type;
		tl_type tl{};
		exp_function_binder bind_efb = exp_bind::bind(make_func_impl(tl));

		auto bind_func = [&bind_efb](auto& value) {bind_efb.bind(&(value.args_stack)); };

		loop_with(*efb_series, bind_func);
		return bind_efb.apply_func();
	}

	template<class ...F>
	auto link_f(F &&...f)
	{
		return link_funcs(exp_bind::bind(f)...);
	}

}
