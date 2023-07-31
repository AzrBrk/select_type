#pragma once
#include<boost/mp11.hpp>
#include<iostream>
#include<tuple>
#include<variant>
#include<vector>
#include<type_traits>
#include<string>
template<class ...Arg_t>
struct exp_list {};

template<size_t N, class L>
struct type_list_size {
	static const size_t value = N + 1;
};
template<class A, class B>
struct add_type {};

template<template<class ...> class A, class B, class ...L>
struct add_type<A<L...>, B> {
	using type = A<L..., B>;
};


template<size_t N, template<class...> class TL, class T, class ...L>
struct type_list_size < N, TL<T, L...>>
{
	static const size_t value = type_list_size<N + 1, TL<L...>>::value;
};

template<size_t N, template<class...> class TL, class T>
struct type_list_size<N, TL<T>>
{
	static const size_t value = N + 1;
};

template<class L>
using size_of_type_list = type_list_size<0, L>;


template<class T>
struct max_type_list_index
{
	static constexpr auto value = size_of_type_list<T>::value - 1;
};
template<class L>
struct split_first
{/*
	using first = int;
	using rest = int;
	using type = int;*/
};

template<template<class...> class TL, class T, class ...L>
struct split_first<TL<T, L...>>
{
	using first = T;
	using rest = TL<L...>;
	using type = TL<T, L...>;
};

template<class T>
struct recover_list
{
	using first_type = split_first<T>::first;
	using rest = split_first<T>::rest;
	using type = split_first<T>::type;
};

template<size_t N, class T>
struct select_type
{
	using type_list = recover_list<T>;
	using first_type = type_list::first_type;
	using lesser_type = type_list::rest;
	using type = select_type<N - 1, lesser_type>::type;
};
template<class T>
struct select_type<0, T>
{
	using first_type = recover_list<T>::first_type;
	using type = first_type;
};


template< bool En, size_t N,class L>
struct safe_select_type_impl
{};
template<size_t N, class L>
struct safe_select_type_impl<true, N, L>
{
	using type = select_type<N, L>;
};
template<size_t N, class L>
struct safe_select_type_impl<false, N, L>
{
	using type = std::false_type;
};
template<size_t N, class L>
struct safe_select_type
{
	using type = safe_select_type_impl <(N > max_type_list_index<L>::value), N, L>::type;
};
template<size_t N, class T>
struct select_type_any
{
	using to_list = boost::mp11::mp_rename<T, exp_list>;
	using type = select_type<N, to_list>::type;
};

template<size_t N, class C, class T>
struct exp_cmp
{
	static const bool value = std::is_same<C, typename select_type<N, T>::type>::value;
};

template<bool bsame, size_t N, class C, class T>
struct is_one_of_type
{};

template<size_t N, class C, class T>
struct is_one_of_type<true, N, C, T>
{
	static const bool value = true;
	static const size_t index = N;
};

template<size_t N, class C, class T>
struct is_one_of_type<false, N, C, T>
{
	static const bool value = is_one_of_type<exp_cmp<N - 1, C, T>::value, N - 1, C, T>::value;
	static const size_t index = is_one_of_type<exp_cmp<N - 1, C, T>::value, N - 1, C, T>::index;
};

template<class C, class T>
struct is_one_of_type<false, 0, C, T>
{
	static const bool value = false;
	//over bondages to fail the compilation, if a false result but is trying to fetch index
	static const size_t index = size_of_type_list<T>::value;
};

template<class C, class T>
using exp_is_one_of
= is_one_of_type <
	exp_cmp<max_type_list_index<T>::value,C, T>::value,
	max_type_list_index<T>::value,
	C, T>;

template<size_t N, class List_type>
struct type_c
{
	using current_int = boost::mp11::mp_append<List_type, exp_list<boost::mp11::mp_int<N>>>;
	using type = type_c<N - 1, current_int>::type;
};

template<class List_type>
struct type_c<0, List_type>
{
	using current_int = boost::mp11::mp_append<List_type, exp_list<boost::mp11::mp_int<0>>>;
	using type = current_int;
};

template<class ...types>
struct type_list
{};
template<class L>
struct generate_index 
{
	using tl = type_c<max_type_list_index<L>::value, type_list<>>::type;
	using type = boost::mp11::mp_rename <tl, exp_list>;
};
template<class T, template<class...> class F>
struct exp_apply
{};
template<template<class...> class L, template<class...> class F, class ...D>
struct exp_apply <L<D...>, F>
{
	using type = L<F<D>...>;
};


template<class L1, class L2, size_t N>
struct exp_combine_impl
{
	using type = exp_list<typename select_type<N, L1>::type, typename select_type<N, L2>::type>;
};

template<bool en, size_t N, class L1, class L2, class List>
struct exp_combine_t
{};

template<size_t N, class L1, class L2, class List>
struct exp_combine_t<true, N, L1, L2, List>
{
	using ct = exp_combine_impl<L1, L2, N>::type;
	using current_type = boost::mp11::mp_append<List, exp_list<ct>>;
	using type = exp_combine_t<true, N - 1, L1, L2, current_type>::type;
};
template<class L1, class L2, class List>
struct exp_combine_t<true, 0, L1, L2, List>
{
	using ct = exp_combine_impl<L1, L2, 0>::type;
	using current_type = boost::mp11::mp_append<List, exp_list<ct>>;
	using type = current_type;
};
template<size_t N, class L1, class L2, class List>
struct exp_combine_t<false, N, L1, L2, List>
{
	using type = std::false_type;//error:length of two lists are not same!!!
};

template<class L1, class L2, template<class...> class Lt = exp_list>
struct exp_combine
{
	using S1 = boost::mp11::mp_int<boost::mp11::mp_size<L1>::value>;
	using S2 = boost::mp11::mp_int<boost::mp11::mp_size<L2>::value>;
	using List = Lt<typename exp_combine_impl<L1, L2, max_type_list_index<L1>::value>::type>;
	using type = exp_combine_t<std::is_same<S1, S2>::value, max_type_list_index<L1>::value -1, L1, L2, List>::type;
};

template<class L1, class L2, template<class...> class Lt>
struct exp_combine_with
{
	using type = exp_combine<L1, L2, Lt>::type;
	//using type = boost::mp11::mp_rename<_type, Lt>;
};

template<class L>
struct index_type_c
{
	using index = generate_index<L>::type;
	using type = exp_combine<index, L>::type;
};
template<class list>
struct list_index
{
	template<size_t N>
	struct index
	{
		static const size_t value = N;
		using type = select_type_any<N, list>::type;
	};
};

template<class T>
struct to_vector {
	using type = std::vector<T>;
};

template<class T>
using to_vector_f = to_vector<T>::type;

template<bool Is_Unique, size_t N, class L>
struct is_unique_type_list_t
{};
template<size_t N, class L>
struct is_unique_type_list_t<false, N, L>//if a type is unique in a list;
{
	using first_type = recover_list<L>::first_type;
	using rest_type = recover_list<L>::rest;
	static const bool value = is_unique_type_list_t<
		exp_is_one_of<first_type, rest_type>::value,//if first type is one of a type in rest types
		N - 1,
		rest_type
	>::value;
};

template<size_t N, class L>
struct is_unique_type_list_t<true, N, L>
{
	static const bool value = false; // if there is a same type, then false
};


template<class L>
struct is_unique_type_list_t<false, 0, L>
{
	static const bool value = true;
};

template<class L>
using is_unique_type_list = is_unique_type_list_t<false, max_type_list_index<L>::value, L>;



template<class L, class en = std::enable_if_t<is_unique_type_list<L>::value>> //every_type in type_list must be unique
struct exp_variable_storage
{
	using Exp_Vector_Type = exp_apply<L, to_vector_f>::type;
	using Tuple_Storage = boost::mp11::mp_rename<Exp_Vector_Type, std::tuple>;

	Tuple_Storage variable_storage{};

	template<class T>
	void push_variable(T const& t)
	{
		auto& type_vec = std::get<to_vector_f<T>>(variable_storage);
		type_vec.push_back(std::move(t));
	}
	template<class T>
	T& get_ref(int index)
	{
		auto& type_vec = std::get<to_vector_f<T>>(variable_storage);

		if (index >= sizeof(type_vec)) throw std::exception("error: index out of range.");

		return type_vec.at(index);
	}
};

template<class T, class variable_storage_t>
struct exp_variable
{
	variable_storage_t& vst;
	int e_index;
	T value() { return vst.get_ref<T>(e_index); }
	template<class U>
	void operator=(U u)
	{
		auto& ref = vst.get_ref<U>(e_index);
		ref = u;
	}
};

template<class variable_storage_t, class T>
exp_variable<T, variable_storage_t> make_variable(variable_storage_t& t, int _index)
{
	return { t, _index };
}

template<
	class T,
	class TL, 
	class En = std::enable_if_t<exp_is_one_of<T,TL>::value>
>
typename safe_select_type<exp_is_one_of<T,TL>::value, TL> get_index(T const& t, TL const& tl)
{
	return {};
}

//这是一个雏形，实现一个类似tuple的容器，这里使用链表结构来模拟
//使用元编程来实现

//element_的四个参数分别表示，它是否是一个终结链表得元素？ 该结点的类型？ 类型列表？ 包装的指针类型？
template<bool End_Type, size_t N, class TL, template<class...> class Pointer_Wrapper>
struct element_ {};

template<size_t N, class TL, template<class...> class Pointer_Wrapper>
struct element_ <false, N, TL, Pointer_Wrapper>
{
	using e_type = typename select_type<N, TL>::type;
	using next_type = element_<N == max_type_list_index<TL>::value - 1, N + 1, TL, Pointer_Wrapper>;
	using next_pointer_type = Pointer_Wrapper<next_type>;

	e_type value;
	operator e_type() { return value; }

	next_pointer_type next = nullptr;

	void set_next(next_pointer_type p) { next = p; }
	next_type& next_element() { return *next; }

	element_(e_type const& e) :value(e) {}

	static const bool has_next = true;
	~element_()
	{}
};
template<size_t N, class TL, template<class...> class Pointer_Wrapper>
struct element_<true, N, TL, Pointer_Wrapper>
{
	using e_type = typename select_type<N, TL>::type;
	e_type value;
	operator e_type() { return value; }
	element_(e_type const& e) :value(e) {}
	static const bool has_next = false;
};

template<size_t N, class TL, template<class...> class Pointer_Wrapper>
using element_node = element_<N == max_type_list_index<TL>::value, N, TL, Pointer_Wrapper>;

template<class Value_Type, class Current_Node_Type, class Constructor>
typename Current_Node_Type::next_pointer_type create_next(Current_Node_Type & _no, Value_Type const& val, Constructor&& con_f)
{
	static_assert(Current_Node_Type::has_next);
	if constexpr (requires(Current_Node_Type cnt) { cnt.next; })
	{
		_no.next = con_f(typename Current_Node_Type::next_type(val), val);
		return _no.next;
	}
	else return nullptr;
}