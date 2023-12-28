#pragma once

#include<memory>
#include"select_type.hpp"
#include<vector>
#include"function_impl.hpp"

namespace possible
{
	template<class node> struct possible_node 
	{
		const size_t idx;
		node first_node;

		exp_iterator<node> iter() {
			exp_iterator it{ first_node };
			it[idx];
			return it;
		}
	};
	
	template<class Node, typename T>
		requires (exp_is_one_of<T, typename Node::element_type_list>::value)
	Node from_value_to_node(T const& v) 
	{
		auto node_maker = [](auto ...x) {
			return make_element_node(shared_constructor(), x...);
			};

		auto efb = exp_bind::bind(function_impl::realize_meta<typename Node::element_type_list>(node_maker));

		size_t idx = exp_is_one_of<T, typename Node::element_type_list>::index;

		efb[idx] = v;
		
		return efb.apply_func();
	}

	//Possiblilities is a class that can stored a series types of data infinitly, the data stored inside can be accessed 
	// by non compiled time index
	//Note: no repeat type in a typelist
	//Note: do not regard this as a repleacement for std::variant, the creation of exp_nodes costs a lot of memory
	template<class ...Typs> 
	class possibilities: public std::vector<possible_node<exp_shared_node<Typs...>>>
	{
	public:
		static_assert(is_unique_type_list<exp_list<Typs...>>::value, "Posibilities requires a typelist with no type repeat");

		template<class T> void push(T const& v) 
		{
			static_assert(exp_is_one_of<T, exp_list<Typs...>>::value, "Provided value is not one of a type from the typelist constructed");
			using possible_type = possible_node<exp_shared_node<Typs...>>;

			this->push_back(possible_type{ 
				exp_is_one_of<T, exp_list<Typs...>>::index,
				from_value_to_node<exp_shared_node<Typs...>>(v) 
				}
			);
		}
		template<typename ...Args> void push(Args... args) {
			(push(std::move(args)), ...);
		}
	};
}