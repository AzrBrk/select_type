#pragma once
#include"join_list.h"
#include<type_traits>
//link multiple function as one, use a same argument stack
namespace exp_function_series
{
	using namespace exp_bind;
	template<class T>
	struct void_wrapper_impl
	{
		using type = T;
	};

	template<>
	struct void_wrapper_impl<void>
	{
		using type = void*;
	};

	template<class T>
	using void_wrapper = typename void_wrapper_impl<T>::type;

	template<class TL>
	struct auto_wrap_void_impl
	{
		using type = typename exp_apply<TL, void_wrapper>::type;
	};

	template<class TL>
	using auto_wrap_void = typename auto_wrap_void_impl<TL>::type;

	template<class EFB>
	using _func_ret_type = typename EFB::return_type;

	template<class EFB>
	using _func_return_void = std::is_same<void, typename EFB::return_type>;


	template<class EFB>
	using _arg_type_list = typename EFB::argument_list_type;//get function argument typelist

	template<class TL>
	using wrap_for_each_TL = typename exp_apply<TL, ref_wrapper>::type;//wrap element is typelist with reference to mulipulate in a common node

	template<class TL>
	using rename_to_tuple = typename exp_rename<TL, std::tuple>::type;//to create a multiple tuple type

	template<class ...EFBs>//type introduct for easy use
	struct series_types
	{
		using arg_list = typename exp_apply<exp_list<EFBs...>, _arg_type_list>::type;
		using ret_list_impl = typename exp_apply<exp_list<EFBs...>, _func_ret_type>::type;
		using ret_list = typename auto_wrap_void<ret_list_impl>;
		using tuples_type = typename exp_apply<arg_list, rename_to_tuple>::type;
		/*using wrapped_arg_list = typename exp_apply<arg_list, wrap_for_each_TL>::type;
		using arg_node_type = typename exp_rename<wrapped_arg_list, exp_shared_node>::type;*/
		using ret_node_type = typename exp_rename<ret_list, exp_shared_node>::type;
		using ret_tuple_type = typename exp_rename<ret_list, std::tuple>::type;
	};

	template<class ...EFBS>//functions link
	struct series
	{
		size_t series_index{};
		size_t argc_index{};
		typename series_types<EFBS...>::ret_tuple_type re_tuple;
		exp_shared_node<EFBS...> func_nodes;
		exp_iterator<exp_shared_node<EFBS...>> func_iter;
		series(exp_shared_node<EFBS...> const& efbs) : func_nodes(efbs), func_iter(func_nodes) {}


		auto& operator*() { return func_nodes; }//get functions node

		series<EFBS...>& argc_at(size_t aridx)
		{
			argc_index = aridx;
			return *this;
		}
		series<EFBS...>& operator++()
		{
			++series_index;
			return *this;
		}

		template<class ...Args>
		void bind(Args...args)
		{
			auto bind_f = []<class EFB>(EFB& efb, auto &&...ars) {
				//efb.reset_argc_stack();
				efb.bind_a_lot(ars...);
			};
			do_at(func_nodes, bind_f, series_index, args...);
		}

		series<EFBS...>& operator[](size_t i) { series_index = i; return *this; }

		void execute() {
			tuple_iterator it{ re_tuple };
			auto apply_f = [this, &it]<class EFB>(EFB& efb)
				{
					if constexpr(_func_return_void<EFB>::value)
					{
						efb.apply_func();
						this->series_index++;
						return;
					}
					else
					{
						it[this->series_index++] = efb.apply_func();
						return;
					}
				};
			do_at(func_nodes, apply_f, series_index);
		}
		void _stop_at(size_t idx)
		{
			for (; series_index <= idx;)
			{
				execute();
			}
		}
		void _continue()
		{
			auto times = func_iter.size();
			for (; series_index < times;)
			{
				execute();
			}
		}
		void bind_last_return()
		{
			tuple_iterator re_iter{ re_tuple };
			auto bind_f = [](auto& value, series<EFBS...>& _this_ref) { 
				_this_ref.bind(value);
			};
			do_at(re_iter.first_node, bind_f, series_index - 1, std::ref(*this));
			//this->bind(19);
		}
		typename series_types<EFBS...>::ret_tuple_type& _return()
		{
			return re_tuple;
		}
	};

	template<class node, class ...L>
	size_t set_node(node& n, L...l)
	{
		size_t idx{};
		auto set_f = [&idx](exp_iterator<node>& it, auto const& value) {it[idx++] = value; };
		exp_iterator itr{ n };
		(set_f(itr, l), ...);
		return idx;
	}

	template<class T>
	using rem_ref = typename std::remove_reference<T>::type;
	template<class ...EFBS>
	auto link_funcs(EFBS&& ...efbs)
	{
		using remove_rf_f =typename exp_apply<exp_list<EFBS...>, rem_ref>::type;
		using series_type = typename exp_rename<remove_rf_f, series>::type;
		series_type efb_series{ make_element_node(shared_constructor(),efbs...) };
		return efb_series;
	}
	//link argument typelist to a common node
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
	//launch links
	template<class ...F>
	auto link_f(F &&...f)
	{
		return link_funcs(exp_bind::bind(f)...);
	}

	template<class ...F>
	auto link_efb(exp_function_binder<F>&& ...efb)
	{
		return link_funcs(efb...);
	}
	//bind series argument
	template<class Series, class ...L>
	void series_bind(Series& srs, L ...l)
	{
		auto argc_node = link_arguments(srs);
		auto set_argc = [&srs]<class ...AL>(auto & node, AL&&...al) { srs.argc_index += set_node(node, al...); };
		do_with_node_at(argc_node, set_argc, srs.argc_index, std::forward<L>(l)...);
	}

}
#define series_empty (void*)0