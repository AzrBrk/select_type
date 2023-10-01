#include"select_type.hpp"

namespace VH_NODE
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

#define member_enum(x,...) enum class x{v,__VA_ARGS__};
#define member_ref(x, m) x.at(static_cast<unsigned int>(x##::##m))
	//a choice of struct instead of C++ struct, this type of struct originally provide reflexing of member
	template<class ...Typs> struct node_struct
	{
		node_struct() = default;
		template<class ...Typs> node_struct(Typs&& ...typs):members(make_vh_shared_nodes(typs...)) {}
		exp_shared_vh_node<Typs...> members{};
		exp_iterator<exp_shared_vh_node<Typs...>> member_iter{ members};
		exp_iterator<exp_shared_vh_node<Typs...>>& at(unsigned int idx)
		{
			return member_iter[idx];
		}
		template<class T> void push_back(T&& val)
		{
			shared_push_node(members, val);
		}
		auto& operator*() { return members.next_element(); }
		size_t size() { return member_iter.size(); }
	};
	template<class ...Typs>
	node_struct(Typs&& ...typs) -> node_struct<Typs...>;
}