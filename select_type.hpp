#pragma once
#include<iostream>
#include<tuple>
#include<variant>
#include<vector>
#include<type_traits>
#include<string>
#include<functional>


struct shared_constructor;
template<class T>
struct check_ref
{
	static const bool  value = false;
};
template<class T>
struct check_ref<T&>
{
	static const bool  value = true;
};

template<class T>
using check_ref_v = check_ref<T>::value;

template<class ...Arg_t>
struct exp_list {};

template<size_t N, class L>
struct type_list_size {
	static const size_t value = N + 1;
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
struct is_empty_list
{
	static const bool value = false;
};
template<template<class...> class TL>
struct is_empty_list<TL<>>
{
	static const bool value = true;
};


template<class T>
struct max_type_list_index
{
	static constexpr auto value = size_of_type_list<T>::value - 1;
};
template<class L>
struct split_first
{};

template<template<class...> class TL, class T, class ...L>
struct split_first<TL<T, L...>>
{
	using first = T;
	using rest = TL<L...>;
	using type = TL<T, L...>;
};

template<size_t I>
struct exp_index
{
	const static size_t value = I;
};

namespace experiment{
	template<bool Ignore, size_t Index, size_t NInc, class TL>
	struct erase_until
	{};
	template<size_t Index, size_t NInc, class TL>
	struct erase_until <true, Index, NInc, TL>
	{
		using type = erase_until<NInc != Index, Index, NInc + 1, typename split_first<TL>::rest>::type;
	};
	template<size_t Index, size_t NInc, class TL>
	struct erase_until <false, Index, NInc, TL>
	{
		using type = TL;
	};

	template<size_t Index, class TL>
	using exp_ignore_until = typename erase_until<Index != 0, Index, 0, TL>::type;

	
}


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
	static const size_t Max_Value = max_type_list_index<T>::value;
	static_assert((N <= Max_Value), "index overflow");

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

template<class TL1, template<class...> class TL2>
struct exp_rename{};

template<template<class ...> class TL1, template<class ...> class TL2, class ...L>
struct exp_rename<TL1<L...>, TL2>
{
	using type = TL2<L...>;
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




template<class L, class T>
struct exp_join_impl
{};

template<template<class...> class L, class T, class ...List>
struct exp_join_impl<L<List...>, T>
{
	using type = L<List..., T>;
};
template<class TL>
struct exp_empty
{};

template<template<class...> class TL, class ...L>
struct exp_empty < TL<L...>>
{
	using type = TL<>;
};
template<class L1, class L2>
struct exp_join
{};

template<class L1, template<class...> class L2, class T, class ...List>
struct exp_join<L1, L2<T, List...>>
{
	using join_type = typename exp_join_impl<L1, T>::type;
	using type =typename exp_join <join_type, L2<List...>>::type;
};
template<class L1, template<class...> class L2>
struct exp_join<L1, L2<>>
{
	using type = L1;
};
template<class ...L>
struct exp_join_a_lot
{};
template<class L1, class L2, class ...L>
struct exp_join_a_lot<L1, L2, L...>
{
	using join_type = typename exp_join<L1, L2>::type;
	using type = typename exp_join_a_lot<join_type, L...>::type;
};
template<class L1, class L2>
struct exp_join_a_lot<L1, L2>
{
	using type = typename exp_join<L1, L2>::type;
};
template<size_t Index, size_t Current_Idx, bool Equal_Length, class TL, class SUB>
struct sub_type_list_impl
{};

template<size_t Index, size_t Current_Idx, template<class...> class TL, class SUB, class T, class ...L>
struct sub_type_list_impl < Index, Current_Idx, false, TL<T, L...>, SUB>
{
	using current_type = typename exp_join_impl<SUB, T>::type;
	using type = typename sub_type_list_impl<Index, Current_Idx + 1, (Index == Current_Idx +1 ), TL<L...>, current_type>::type;
};
template<size_t Index, size_t Current_Idx, template<class...> class TL, class SUB, class ...L>
struct sub_type_list_impl <Index, Current_Idx, true, TL<L...>, SUB>
{
	using type = SUB;
};

template<size_t Index, class TL>
struct Test_Index
{
	static_assert((Index <= max_type_list_index<TL>::value + 1), "Test Index Error :index overflow");
	static const size_t value = Index;
};



template<size_t Index, class TL>
using sub_type_list = sub_type_list_impl<Test_Index<(Index), TL>::value, 0, (Index) == 0, TL, typename exp_empty<TL>::type>::type;

namespace experiment{
	template<size_t Index, class mtl>
	struct erase_at_t
	{
		static_assert(Index <= max_type_list_index<mtl>::value - 1);
		using front = sub_type_list<Index - 1, mtl>;
		using back = exp_ignore_until<Index + 1, mtl>;
		using type = typename exp_join<front, back>::type;
	};
	template<class mtl>
	struct erase_at_t<0, mtl>
	{
		using type = typename split_first<mtl>::rest;
	};
}



template<class T, template<class...> class F>
struct exp_apply
{};
template<template<class...> class L, template<class...> class F, class ...D>
struct exp_apply <L<D...>, F>
{
	using type = L<F<D>...>;
};


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


template<bool End_Type, size_t N, class TL, template<class...> class Pointer_Wrapper>
struct element_ {};

template<size_t N, class TL, template<class...> class Pointer_Wrapper>
struct element_ <false, N, TL, Pointer_Wrapper>
{
	using element_type_list = TL;
	using e_type = typename select_type<N, TL>::type;
	using next_type = element_<N == max_type_list_index<TL>::value - 1, N + 1, TL, Pointer_Wrapper>;
	using next_pointer_type = Pointer_Wrapper<next_type>;
	static const size_t Index = N;
	static const size_t capacity= max_type_list_index<TL>::value;
	e_type value;
	e_type& value_ref() { return value; }
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
	using element_type_list = TL;
	using e_type = typename select_type<N, TL>::type;

	static const size_t Index = N;
	static const size_t capacity = max_type_list_index<TL>::value;

	e_type value;
	e_type& value_ref() { return value; }
	operator e_type() { return value; }
	element_(e_type const& e) :value(e) {}
	static const bool has_next = false;
};


template<size_t N, class TL, template<class...> class Pointer_Wrapper>
using element_node = element_<N == max_type_list_index<TL>::value, N, TL, Pointer_Wrapper>;

template<template<class...> class PW = std::shared_ptr, class ...TL>
using exp_node = element_node<0, exp_list<TL...>, PW>;

template<class ...L>
using exp_shared_node = exp_node<std::shared_ptr, L...>;

template<class Value_Type, class Current_Node_Type, class Constructor>
typename Current_Node_Type::next_pointer_type create_next(Current_Node_Type & _no, Value_Type const& val, Constructor&& con_f)
{
	//static_assert(Current_Node_Type::has_next);
	if constexpr (requires(Current_Node_Type cnt) { cnt.next; })
	{
		_no.next = con_f.template operator()<typename Current_Node_Type::next_type>(val);
		return _no.next;
	}
	else return nullptr;
}

template<class T>
using test_type = typename T::type;

template<class Nextable>
struct forwarder
{
	using type = typename Nextable::next_type;
};

template<class T, class R, class ...L>
struct function_forwarder
{
	using type = std::function<R(typename forwarder<T>::type&, L...)>;
	type call;
};




template<class _node, class T, class constructor_f>
void push_back(_node& _n, T const& value, constructor_f&& cf)
{
	if constexpr (requires(_node _nn) { _nn.next; })
	{
		if (!_n.next)
		{
			create_next(_n, value, cf);
			return;
		}
	}
	if constexpr (_node::has_next == true)
	{
		function_forwarder<_node, void, T, constructor_f> ff;
		ff.call = push_back<
			typename forwarder<_node>::type,
			T, constructor_f
		>;
		ff.call(*(_n.next), value, cf);
	}
}

template<class T>
struct is_wrapped
{
	static const bool value = false;
};

template<template<class> class wrapper, class T>
struct is_wrapped<wrapper<T>>
{
	static const bool value = true;
	using type = T;
};
template<class X, class Y>
void exp_assign(X& x, Y const& y)
{
	if constexpr (std::is_same_v<X, Y>)
	{
		x = y;
		return;
	}
	if constexpr (is_wrapped<X>::value)
	{
		if constexpr (std::is_same_v<typename is_wrapped<X>::type, Y>)
		{
			x = y;
			return;
		}
	}
	if constexpr (is_wrapped<Y>::value)
	{
		if constexpr (std::is_same_v<X, typename is_wrapped<Y>::type>)
		{
			if constexpr (requires(Y v) { v.value; })
			{
				x = y.value;
				return;
			}
			std::cout << "warning:there is no Y::value, it is not a member of Y" << std::endl;
		
		}
	}
	std::cout << "warning:type dismatched:" << typeid(x).name() << "," << typeid(y).name() << std::endl;
}
template<class _node, class T>
void assign_at(_node& _n, T const& value, size_t idx)
{
	if (!idx) { exp_assign(_n.value, value); return; }
	if constexpr (_node::has_next == true)
	{
		if (!_n.next && idx) { std::cout << "fatal error: index overflow!"; return; }
	}
	if constexpr (_node::has_next == true)
	{
		function_forwarder<_node, void, T, size_t> ff;
		ff.call = assign_at<typename forwarder<_node>::type, T>;
		ff.call(_n.next_element(), value, --idx);
	}
}

template<class _node, class func, class...L>
void do_at(_node& _n, func&& f, size_t idx, L...l)
{
	if (!idx) {
		f(_n.value_ref(), std::forward<L>(l)...);
		return;
	}
	if constexpr (_node::has_next == true)
	{
		if (!_n.next && idx) { std::cout << "fatal error: index overflow!"; return; }
	}
	if constexpr (_node::has_next == true)
	{
		function_forwarder<_node, void, func, size_t, L...> ff;
		ff.call = do_at<typename forwarder<_node>::type, func, L...>;
		ff.call(_n.next_element(), f, --idx, std::forward<L>(l)...);
	}
}
template<class _node, class func, class...L>
void do_with_node_at(_node& _n, func&& f, size_t idx, L...l)
{
	if (!idx) { 
		f(_n, std::forward<L>(l)...);
		return; 
	}
	if constexpr (_node::has_next == true)
	{
		if (!_n.next && idx) { std::cout << "fatal error: index overflow!"; return; }
	}
	if constexpr (_node::has_next == true)
	{
		function_forwarder<_node, void, func, size_t, L...> ff;
		ff.call = do_with_node_at<typename forwarder<_node>::type, func, L...>;
		ff.call(_n.next_element(), f, --idx, l...);
	}
}

template<class _node, class func, class...L>
void loop_with(_node& _n, func&& f, L...l)
{
	f(_n.value, l...);
	if constexpr (_node::has_next == true)
	{
		if (!_n.next) return;
		function_forwarder<_node, void, func, L...> ff;
		ff.call = loop_with<typename forwarder<_node>::type, func, L...>;
		ff.call(_n.next_element(), f, l...);
	}
}



template<class _node>
struct exp_iterator
{
	_node& first_node;
	size_t exp_index{ 0 };


	exp_iterator(_node& _n) :first_node(_n) {}


	template<class T>
	T operator=(T const& value) { 
		assign_at(first_node, value, exp_index); 
		return value; 
	}
	exp_iterator<_node>& operator=(const exp_iterator<_node> & it)
	{
		auto node_transfer = [this](auto const& value) {(*this) = value; };
		do_at(it.first_node, node_transfer, it.exp_index);
		return *this;
	}

	void operator++()
	{
		++exp_index;
	}
	exp_iterator<_node>& operator[](size_t i) 
	{
		exp_index = i;
		return *this;
	}
	
	void reset() { exp_index = 0; }
	size_t size()
	{
		size_t s{ 0 };
		auto get_size = [&s](auto && ...l)
			{
				++s;
			};
		loop_with(first_node, get_size);
		return s;
	}

	template<class T>
	exp_iterator(T const& value) :first_node(value) {}

	template<class T, class construct_f>
	void exp_push_back(T const& value, construct_f&& f)
	{
		push_back(first_node, value, f);
	}
	friend std::istream& operator >>(std::istream& is, exp_iterator<_node>& _n)
	{
		auto input = [&is](auto& value)->void
			{
				is >> value;
			};
		do_at(_n.first_node, input, _n.exp_index);
		return is;
	}
	friend std::ostream& operator <<(std::ostream& os, exp_iterator<_node>& _n)
	{
		auto output = [&os](auto& value)->void
			{
				os << value;
			};
		do_at(_n.first_node, output, _n.exp_index);
		return os;
	}
};

template<class _node>
exp_iterator(_node& n) -> exp_iterator<_node>;

template<class T, class Exp_iter>
T fetch_value(T& va, Exp_iter& it)
{
	auto fetch = [&va](auto value) { exp_assign(va, value); };
	do_at(it.first_node, fetch, it.exp_index);
	return va;
}


template<class T, class ...L> 
element_node < 0, exp_list<T, L...>, std::shared_ptr> make_element_node(auto&& constructor, T const& t, L...l)
{
	element_node < 0, exp_list<T, L...>, std::shared_ptr> first_node(t);
	(::push_back(first_node, l, constructor), ...);
	return first_node;
}
template<class T>
struct ref_wrapper
{
	T& value;
	ref_wrapper(T& _v) :value(_v) {};
	ref_wrapper(const ref_wrapper& rw) :value(rw.value) {}
	T& operator=(T const& v) { value = v; return value; }
	ref_wrapper& operator=(const ref_wrapper& rw) { value = rw.value; return *this; }
	//operator T& () { return value; }
	operator T () { return value; }
	friend std::istream& operator>>(std::istream& is, ref_wrapper<T>& rw)
	{
		if constexpr (requires(std::istream & i, ref_wrapper<T> r) { i >> r.value; })
			is >> rw.value;
		return is;
	}
	friend std::ostream& operator<<(std::ostream& os, ref_wrapper<T>& rw)
	{
		os << rw.value;
		return os;
	}
};

template<class T>
struct auto_ref_unwrapper_impl
{
	using type = T;
};
template<class T>
struct auto_ref_unwrapper_impl<ref_wrapper<T>>
{
	using type = T;
};

template<class T>
using auto_ref_unwrapper = typename auto_ref_unwrapper_impl<T>::type;


template<class Tuple>
struct tuple_iterator_types
{
	using node_list_type_impl = typename exp_apply<Tuple, ref_wrapper>::type;

	using node_list_type = typename exp_rename<node_list_type_impl, exp_list>::type;

	using first_node_type = element_node<0, node_list_type, std::shared_ptr>;

	using iterator_type = exp_iterator<first_node_type>;
};

template<class _node, class Tuple> void get_from_tuple(_node& _n, Tuple& tp)
{
	auto shr_con = []<class Ty, class T>(T & value) { return std::make_shared<Ty>(value); };

	if constexpr (_node::has_next && (_node::Index < max_type_list_index<Tuple>::value))
	{
		auto rwp = ref_wrapper(std::get <_node::Index + 1>(tp)) ;
		create_next(_n, rwp, shared_constructor());
		function_forwarder<_node, void, Tuple&> ff;
		ff.call = get_from_tuple<typename forwarder<_node>::type, Tuple>;
		ff.call(_n.next_element(), tp);
	}
}
template<class Tuple>
struct tuple_iterator
{
	typename tuple_iterator_types<Tuple>::first_node_type first_node;
	using iterator_type = typename tuple_iterator_types<Tuple>::iterator_type;

	iterator_type __tuple_iterator;
	Tuple& _tp;
	tuple_iterator(Tuple& tp) :_tp(tp), first_node(std::get<0>(tp)), __tuple_iterator(first_node) { initialize(); }
	void initialize()
	{
		get_from_tuple(first_node, _tp);
	}
	iterator_type& iterator() { return __tuple_iterator; }
	iterator_type& operator[](size_t index) { return __tuple_iterator[index]; }
};

template<class ...L>
tuple_iterator(std::tuple<L...>& tp) -> tuple_iterator<std::tuple<L...>>;

struct shared_constructor
{
	template<class Ty, class T>
	std::shared_ptr<Ty> operator()(T const& value)
	{
		if constexpr (std::is_same<typename Ty::e_type, T>::value)
		{
			return construct<Ty>(value);
		}
		else {
			std::cout << "warning: type_dismatched: " << typeid(typename Ty::e_type).name() << "," << typeid(value).name() << std::endl;
			return nullptr;
		}
	}

	template<class Ty, class T>
	std::shared_ptr<Ty> construct(T const& value){
		return std::make_shared<Ty>(value);
	}
};

struct make_node_shared
{
	template<class _node>
	auto operator()(_node&& n)
	{
		auto sh_node = std::make_shared<typename std::remove_reference<_node>::type>(n.value);
		if constexpr (std::remove_reference<_node>::type::has_next)
		{
			exp_assign(sh_node->next, n.next);
		}
		return sh_node;
	}
};


