#include<vector>
#include<any>
#include<iostream>
#include<memory>
#include"select_type.hpp"
#include<type_traits>


using namespace std;



template<class ...L>
struct my_type_list {};

template<class A, class B>
struct add_type {};

template<template<class ...> class A, class B, class ...L>
struct add_type<A<L...>, B> {
	using type = A<L..., B>;
};

template<bool End_Type, size_t N, class TL>
struct element_ {};

template<size_t N, class TL>
struct element_ <false,N,TL>
{
	using e_type = typename select_type<N, TL>::type;
	using next_type = element_<N == max_type_list_index<TL>::value - 1 , N+1, TL > ;
	e_type value;
	operator e_type() { return value; }

	next_type* next = nullptr;
	next_type next_element() { return *next; }

	element_(e_type const& e) :value(e) {}

	static const bool has_next = true;
	~element_()
	{}
};
template<size_t N, class TL>
struct element_<true, N, TL>
{
	using e_type = typename select_type<N, TL>::type;
	e_type value;
	operator e_type() { return value; }
	element_(e_type const& e) :value(e) {}
	static const bool has_next = false;
};

template<size_t N, class T>
struct element_to_ {};

template<size_t I,bool End_Type, size_t N, class TL>
struct element_to_<I, element_<End_Type, N, TL>>
{
	using type = element_to_<I - 1, typename element_<End_Type, N, TL>::next_type>::type;
};
template<bool End_Type, size_t N, class TL>
struct element_to_<0, element_<End_Type, N, TL>>
{
	using type = element_<End_Type, N, TL>;
};
template<class T, template<class...> class Pointer_Wrapper = std::shared_ptr>
struct element_iterator_ {};

template<size_t N, class TL, template<class...> class Pointer_Wrapper>
struct element_iterator_<element_<true, N, TL>, Pointer_Wrapper>
{
	using current_element_type_ = element_<true, N, TL>;
	using current_keeper = Pointer_Wrapper<current_element_type_>;
	using value_type = typename current_element_type_::e_type;

	current_keeper _node{ nullptr };
	element_iterator_() = default;
	element_iterator_(auto f, auto const& value) { _node = f(current_element_type_(value), value); }
};

template<size_t N, class TL, template<class...> class Pointer_Wrapper>
struct element_iterator_<element_<false, N, TL>, Pointer_Wrapper>
{
	using current_element_type_ = element_<false, N, TL>;
	using current_keeper_type = Pointer_Wrapper<current_element_type_>;

	using next_element_type_ = current_element_type_::next_type;
	using next_iterator_type = element_iterator_<next_element_type_, Pointer_Wrapper>;
	using next_keeper_type = Pointer_Wrapper<next_element_type_>;

	using value_type = typename current_element_type_::e_type;

	next_keeper_type next_node{ nullptr };
	current_keeper_type _node{ nullptr };
	element_iterator_() = default;
	element_iterator_(auto f, auto const& value){_node = f(current_element_type_(value), value);}
	next_iterator_type push_back(auto f, auto const& value) {
		next_iterator_type next_iter{ f, value };
		next_node = next_iter._node;
		_node->next = next_node.get();
		return next_iter;
	}
	current_element_type_& operator*() { return *_node; }
	operator value_type() { return _node->operator(); }
	value_type operator()() { return _node->value; }
};
template<size_t N, class TL>
using to_element_node = element_<((N + 1) > max_type_list_index<TL>::value), N, TL > ;

template<size_t N, class TL>
void print_next(element_<false, N, TL> iter)
{
	print_element_list(iter);
}
template<size_t N, class TL>
void print_next(element_<true, N, TL> iter)
{
	cout << iter.value << endl;
}

template<bool End_Type, size_t N, class TL>
void print_element_list(element_<End_Type, N, TL> iter)
{
	using node_type = element_<End_Type, N, TL>;
	cout << iter.value << " ";
	if constexpr (requires(node_type n) {
			n.next; n.next_element();
		}
	)
	{
		if (iter.next) print_next(iter.next_element());
	}
}


int main()
{
	auto share_constructor = [](auto ty, auto const& value) { return std::make_shared<decltype(ty) >(value); };
	using mtl = my_type_list<int, double, char, unsigned>;
	using first_node_type = to_element_node<0, mtl>;
	element_iterator_<first_node_type, std::shared_ptr> first_iter(share_constructor, 1);
	auto second_iter = first_iter.push_back(share_constructor, 2.33);
	auto third_iter = second_iter.push_back(share_constructor, 'c');
	auto fourth_iter = third_iter.push_back(share_constructor, 88000);
	//using errror____ =element_<true, 7, mtl>::e_type;
	print_element_list(*first_iter);
	return 0;
}