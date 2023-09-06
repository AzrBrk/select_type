#pragma once
#include"exp_function_binder.hpp"

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

	template<class node, class wrapper>
	void next_impl(node& n, wrapper& rw)
	{
		auto shr_con = []<class Ty, class T>(T & value) { 
			return std::make_shared<Ty>(value); 
		};
		if constexpr (std::is_same_v<typename node::next_type::e_type, wrapper>)
		{
			create_next(n, rw, shr_con);
		}
	}

	template<class node, size_t Index, class _tuple>
	void fill_from_tuple(node& n, _tuple& tp)
	{
		//auto shr_con = []<class Ty, class T>(T & value) { return std::make_shared<Ty>(value); };
		if constexpr (node::has_next && Index <= max_type_list_index<_tuple>::value)
		{
			auto rwp = ref_wrapper(std::get<Index>(tp));
			next_impl(n, rwp);
			function_forwarder<node, void, _tuple&> ff;
			ff.call = fill_from_tuple<typename forwarder<node>::type, Index + 1, _tuple>;
			ff.call(n.next_element(), tp);
		}
	}

	template<class node, template<class...> class _tuple, class ...L>
	auto link_from_tuple(node& n,auto& _otp, _tuple<L...>& tp)
	{
		using element_type_list_impl = typename exp_apply<
			typename node::element_type_list, 
			auto_ref_unwrapper>::type; //convert tuple made node type to original type list


		using join_type = typename exp_join<
			element_type_list_impl, 
			_tuple<L...>>::type; //create typelist to construct tuple

		using join_tuple_type = typename exp_rename<
			join_type, 
			std::tuple>::type; // define tuple type

		// get first node type
		using first_node_type = typename tuple_iterator_types<join_tuple_type>::first_node_type;
		first_node_type jttiter{ n.value };

		auto jtt_node_ = jttiter;
		get_from_tuple(jtt_node_, _otp);

		//visit lambda
		auto do_node = []<class node, class _ltuple>(node& n, _ltuple tp) {
			return fill_from_tuple <node, 0, _tuple<L...>>(n, tp);
		};
		auto pos = exp_iterator<node>(n).size() - 1;
		do_with_node_at(jtt_node_, do_node, pos, std::ref(tp));

		return jtt_node_;
	}

	/// <summary>
	/// Create link nodes contains references of every element within the two nodes.
	/// While keeping them separated.
	/// </summary>
	/// <param name="tp1">A tuple</param>
	/// <param name="tp2">Another tuple</param>
	/// <returns>Element_ node</returns>
	auto link_tuples(auto& tp1, auto& tp2)
	{
		tuple_iterator<typename std::remove_reference<decltype(tp1)>::type> tp1_iter{ tp1 };
		return link_from_tuple(tp1_iter.iterator().first_node, tp1, tp2);
	}

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
	template<class _node>
	auto unwrapped_node(_node& _n)
	{
		using _node_type = typename exp_apply<typename _node::element_type_list, auto_ref_unwrapper>::type;
		using _node_impl = typename exp_rename<_node_type, exp_shared_node>::type;
		_node_impl new_node(_n.value.value);
		exp_iterator pre_iter{ _n };
		exp_iterator aft_iter{ new_node };
		size_t i = 0;
		auto loop_func = [&aft_iter](auto& value) {
			aft_iter.exp_push_back(value.value, shared_constructor()); 
			};
		loop_with(_n.next_element(), loop_func);
		return new_node;
	}

	template<class _node>
	auto node_to_tuple(_node & n)
	{
		using tuple_type_list = typename exp_apply<typename _node::element_type_list, auto_ref_unwrapper>::type;
		using tuple_type = typename exp_rename<tuple_type_list, std::tuple>::type;

		tuple_type tp;

		tuple_iterator<decltype(tp)> tp_it(tp);
		auto tp_iter = tp_it.iterator();
		size_t index = 0;
		auto assign_tuple = [&tp_iter, &index](auto& value) {
			tp_iter[index++] = value;
			};
		loop_with(n, assign_tuple);
		return tp;
	}
	template<class NODE, class TP>
	void tuple_fill(NODE& node, TP& tp)
	{
		auto pos = exp_iterator<NODE>(node).size() - 1;
		//visit lambda
		auto do_node = []<class node, class _ltuple>(node & n, _ltuple tp) {
			return fill_from_tuple <node, 0, TP>(n, tp);
		};
		do_with_node_at(node, do_node, pos, std::ref(tp));
	}

	template<class TP, class...TPS>
	auto link_a_lot_tuples(TP& tp, TPS&...tps)
	{
		using node_type_impl = typename exp_rename<typename exp_join_a_lot<TP, TPS...>::type, exp_list>::type;
		using node_type_list = typename exp_apply<node_type_impl, ref_wrapper>::type;
		using node_type = element_node<0, node_type_list, std::shared_ptr>;
		node_type node(ref_wrapper(std::get<0>(tp)));
		get_from_tuple(node, tp);
		(tuple_fill(node, tps), ...);
		return node;
	}	
}
