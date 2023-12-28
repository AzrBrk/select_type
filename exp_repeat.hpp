#pragma once
#include"select_type.hpp"
#include"exp_function_binder.hpp"
#include <type_traits>
#include <memory>

namespace exp_repeat
{

	template<class F, class ...L>
	struct Meta_Invoke {
		using type = F::template apply<L...>;
	};

	template<class F, class ...L>
	using meta_invoke = typename Meta_Invoke<F, L...>::type;

	struct below_zero {
		static constexpr size_t value = 0;
	};

	template<size_t _I>
	struct Idx {
		static const size_t value = _I;
	};

	template<class _Idx, size_t _I>
	struct Add_Idx {};

	template<size_t I> struct Add_Idx<below_zero, I>
	{
		using type = Idx<0>;
	};

	template<template<size_t> class _Idx, size_t _I_in_Idx, size_t _I>
	struct Add_Idx<_Idx<_I_in_Idx>, _I>
	{
		using type = _Idx<_I_in_Idx + _I>;
	};

	template<class _Idx, size_t _I>
	using add_idx_t = typename Add_Idx<_Idx, _I>::type;

	template<class _Idx>
	using inc_idx_t = add_idx_t<_Idx, 1>;


	template<size_t _stride_length>
	struct _Stride {};

	template<bool _Condition, class _Meta_Object, class F, class Arg_t>
	struct looper
	{
		using _current_type = meta_invoke<_Meta_Object, meta_invoke<F, Arg_t>>;
		template<class _Bool_Fn>
		struct apply
		{
			using type = typename looper<
				meta_invoke<_Bool_Fn, _Meta_Object>::value, _current_type,
				F, meta_invoke<F, Arg_t>
			>::template apply<_Bool_Fn>::type;
		};
	};
	template<class _Meta_Object, class F, class Arg_t>
	struct looper<false, _Meta_Object, F, Arg_t>
	{
		template<class _Bool_Fn> struct apply{ using type = _Meta_Object; };
	};

	template<size_t _Limit>
	struct _Length_Less_Than
	{
		template<class TL> struct apply{ static const bool value = (TL::length < _Limit); };
	};

	template<class TL>
	struct _Appendable_Meta_List
	{
		static const size_t length = size_of_type_list<TL>::value;
		using type = TL;
		template<class _Extent>
		using apply = _Appendable_Meta_List<typename exp_join_impl<TL, _Extent>::type>;
	};

	struct _Increase_Idx
	{
		template<class _Idx> using apply = inc_idx_t <_Idx>;
	};


	

	template<size_t NUM>
	using meta_itoa = typename looper<(NUM != 0), _Appendable_Meta_List<exp_list<exp_repeat::Idx<0>>>,
		_Increase_Idx, exp_repeat::Idx<0>>::template apply<_Length_Less_Than<NUM>>::type::type;

	template<size_t ..._NUM>
	struct meta_array
	{
		using cv_typelist = exp_list<Idx<_NUM>...>;
		template<template<size_t...I> class integer_array_type>
		using to = integer_array_type<_NUM...>;
		template<size_t N> using at = exp_select<N, cv_typelist>;
	};

	template<class TL>
	struct _Meta_To_Array{};

	template<template<class...> class TL, size_t... _I>
	struct  _Meta_To_Array<TL<Idx<_I>...>>
	{
		using type = meta_array<_I...>;
	};

	template<class TL>
	using meta_to_array =typename _Meta_To_Array<TL>::type;

	namespace invoke
	{
		template<size_t _Idx, class _Node>
		exp_select<_Idx, typename _Node::element_type_list> get_from_node(_Node& node)
		{
			exp_select<_Idx, typename _Node::element_type_list> val{};
			return fetch_value(val, exp_iterator<_Node>(node)[_Idx]);
		}
		template<size_t _Idx, class _Node>
		exp_select<_Idx, typename _Node::element_type_list>& ref_from_node(_Node& node)
		{
			using ref_type = exp_select<_Idx, typename _Node::element_type_list>&;
			exp_reference_pointer<ref_type> erp{};
			auto get_ref = [&erp]<class vt>(vt& value) {
				if constexpr (requires(vt& v){erp = v; })
					erp = value;
			};
			do_at(node, get_ref, _Idx);
			return (ref_type)erp;
		}

		template<template<size_t ...> class _Meta_Array, class F, class _Node, size_t ...idxs>	
		auto node_invoke(_Meta_Array<idxs...> metas, _Node& node, F&& f)
		{
			return f((get_from_node<idxs>(node))...);
		}
		template<class _Iota_Type>
		struct _Itoa_To_Array {};

		template<size_t ..._Nums>
		struct _Itoa_To_Array<exp_list<Idx<_Nums>...>>
		{
			using type = meta_array<_Nums...>;
		};

		template<size_t _Max>
		using meta_ordered_array_c = typename  _Itoa_To_Array<meta_itoa<_Max>>::type;

		template<class _Node, class F>
		auto node_invoke(_Node& node, F&& f)
		{
			return node_invoke(meta_ordered_array_c<max_type_list_index<typename _Node::element_type_list>::value>(), node, f);
		}
#define VERIFY_MUTABLE_ARGMENT(NAME) if constexpr(sizeof...(NAME)!=0)
		template<class _Node, class ...L>
		void push_invoke(_Node& node, L...l)
		{
			VERIFY_MUTABLE_ARGMENT(l)
				(push_back(node, l, shared_constructor()), ...);
		}
		template<size_t N> struct idx_invoke {
			using idx_sequence = meta_to_array<meta_itoa<N>>;

			template<class F, size_t ...I, class ...Args> auto constexpr invoke_impl(meta_array<I...>, F&& f, Args ...args) const
			{
				return f.template operator() < I... > (std::forward<Args>(args)...);
			}

			template<class F, class ...Args> auto constexpr operator()(F&& f, Args&&...args) const {
				return invoke_impl(idx_sequence{}, std::forward<F>(f), std::forward<Args>(args)...);
			}
		};

	}
	template<auto fn>
	using argument_node = element_node<0, 
		typename decltype(
			exp_bind::bind(fn)
			)::argument_list_type, std::shared_ptr>;

	template<class ...>
	struct Meta_Expr {};
	template<class _Idx1, class _OP, class _Idx2>
	struct Meta_Expr<_Idx1, _OP, _Idx2> {
		using type = meta_invoke<_OP, _Idx1, _Idx2>;
	};
	namespace operators{

		template<const char _O> struct Meta_Op {};
		template<> struct Meta_Op<'+'> {
			template<class X, class Y>
			using apply = Idx<X::value + Y::value>;
		};
		template<> struct Meta_Op<'-'> {
			template<class X, class Y>
			using apply = Idx<X::value - Y::value>;
		};
		template<> struct Meta_Op<'*'> {
			template<class X, class Y>
			using apply = Idx<X::value * Y::value>;
		};
		template<> struct Meta_Op<'>'> {
			template<class X, class Y>
			struct apply { static const bool value = (X::value > Y::value); };
		};
		template<> struct Meta_Op<'='> {
			template<class X, class Y>
			struct apply { static const bool value = (X::value == Y::value); };
		};
		using plus = Meta_Op<'+'>;
		using sub = Meta_Op<'-'>;
		using mul = Meta_Op<'*'>;
		using greater = Meta_Op<'>'>;	
		using equal = Meta_Op<'='>;
	}

	template<class ...L>
	using meta_expr = typename Meta_Expr<L...>::type;

	template<class ...L>
	using meta_expr_string = Meta_Expr<L...>;

}