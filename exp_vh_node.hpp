#include"select_type.hpp"
#include"exp_function_series.hpp"
#include"meta_selectable_list.hpp"
#include"function_impl.hpp"
#include<concepts>
#include<string>
#include<iostream>
#include <type_traits>
#include<optional>


namespace vh_node
{
	struct void_node {};
	template<class TL, template<class...> class PW>
	struct vh_element_node {};

	template<template<class...> class TL, template<class ...> class PW, class ...T>
	struct vh_element_node<TL<T...>, PW> : element_<sizeof...(T) == 0, 0, TL<void_node, T...>, PW> {};
	template<template<class ...> class PW, class...Typs>
	using exp_vh_node = vh_element_node<exp_list<Typs...>, PW>;

	template<class ...Typs> using exp_shared_vh_node = exp_vh_node<std::shared_ptr, Typs...>;

	template<class ...Typs> auto make_vh_shared_nodes(Typs const& ...typs)
	{
		return make_element_node(shared_constructor(),void_node{}, typs...);
	}

	template<class _node, class T> auto shared_push_node(_node& n, T&& value)
	{
		return ::push_back(n, value, shared_constructor());
	}
	template<class Node> constexpr bool node_has_next = std::decay_t<Node>::has_next;

	struct next_node
	{
		template<typename Node> auto operator()(Node&& node) const
		{
			if constexpr (node_has_next<Node>)
			{
				return std::move(node.next_element());
			}
			else return node;
		}
	};

#define member_enum(x,...) enum class x##_ifo{__VA_ARGS__};
#define member_ref(x, m) x.at(static_cast<unsigned int>(x##_ifo::##m))

	//a choice of struct instead of C++ struct, this type of struct originally provide reflexing of member
	template<class ...Typs> struct node_struct_base
	{
		node_struct_base() = default;
		node_struct_base(Typs&& ...typs):members(make_vh_shared_nodes(typs...)) {}
		template<bool end_type, size_t I, class TL, template<class...> class Pw>
		node_struct_base(element_<end_type, I, TL, Pw>&& node) : members(node) {}
		exp_shared_vh_node<Typs...> members{};
		exp_iterator<exp_shared_vh_node<Typs...>> member_iter{ members};
		exp_iterator<exp_shared_vh_node<Typs...>>& at(unsigned int idx)
		{
			return member_iter[idx + 1];//first element ignored, since it is a root node
		}
		template<class T> void push_back(T&& val)
		{
			shared_push_node(members, val);
		}
		auto& operator*() { return members.next_element(); }
		size_t size() { return member_iter.size(); }
	};

	template<bool end_type, size_t I, class vh, template<class ...> class TL, template<class ...> class Pw, class ...Args>
	node_struct_base(element_<end_type, I, TL<vh, Args...>, Pw>&&) -> node_struct_base<Args...>;

	using namespace function_impl;
	template<class...Typs>
	struct node_struct : node_struct_base<Typs...> 
	{
		node_struct() {}
		node_struct(Typs&& ...typs) :node_struct_base<Typs...>(std::move(typs)...) {}
		template<bool ET, size_t I, template<class...> class TL, class vh, class ...Args, template<class...> class Pw>
		node_struct(element_<ET, I, TL<vh, Args...>, Pw>&& on) : node_struct_base<Args...>(std::move(on)) {}
	};
	
	
	template<class ...Typs>
	node_struct(Typs&& ...typs) -> node_struct<Typs...>;

	template<bool ET, size_t I, template<class...> class TL, class vh, class ...Args, template<class...> class Pw>
	node_struct(element_<ET, I, TL<vh, Args...>, Pw>&& on) -> node_struct<Args...>;
}