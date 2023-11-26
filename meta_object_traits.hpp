#pragma once
#include "exp_repeat.hpp"
#include <type_traits>
namespace meta_traits
{
#define MMO(t) m_meta_object_##t
	using namespace exp_repeat;
	using namespace exp_repeat::operators;

	template<class F, class TL>
	struct exp_fn_apply_impl {};

	template<class F, template<class ...> class TL, class ...L>
	struct exp_fn_apply_impl<F, TL<L...>>
	{
		using type = TL<meta_invoke<F, L>...>;
	};

	template<class F, class TL> using exp_fn_apply = typename exp_fn_apply_impl<F, TL>::type;

	struct meta_empty {};
	struct meta_empty_fn { template<class T, class ...> using apply = T;};


	
	/*A meta obj is a bind of a meta_function and an obj, each time it is invoked, it update itself to a new type,
	use ::type to get the inner obj*/
	template<class MMO(Obj), class F/*Define how to Update an obj*/>
	struct meta_object
	{
		using type = MMO(Obj);
		template<class ...Arg>
		using apply = meta_object<meta_invoke<F, MMO(Obj), Arg...>, F>;

		template<class Outer_Obj>
		using meta_set = meta_object<Outer_Obj, F>;
	};

	template<class MMO(Obj), class F, class Ret>
	struct meta_ret_object 
	{
		using ret = meta_invoke<Ret, MMO(Obj)>;
		using type = MMO(Obj);
		template<class ...Arg>
		using apply = meta_ret_object<meta_invoke<F, MMO(Obj), Arg...>, F, Ret>;

		template<class Outer_Obj>
		using meta_set = meta_ret_object<Outer_Obj, F, Ret>;
	};

	template<size_t times, class MMO(Obj), class F>
	struct meta_timer_object
	{
		template<size_t ts> struct timer_
		{
			static const bool value = (ts > 0);
		};
		using timer = timer_<times>;
		using type = MMO(Obj);
		template<class ...Arg>
		using apply = meta_timer_object<times - 1, meta_invoke<F, MMO(Obj), Arg...>, F>;

		template<class Outer_Obj>
		using meta_set = meta_timer_object<times, Outer_Obj, F>;
	};


	template<class MMO(From), class MMO(To)>
	struct meta_transfer {
		using type = typename MMO(To)::template meta_set<typename MMO(From)::type>;
	};

	template<size_t times, class obj, class F, class MMO(To)>
	struct meta_transfer<meta_timer_object<times, obj, F>, MMO(To)> {
		using type = typename MMO(To)::template meta_set<typename meta_timer_object<times, obj, F>::timer>;
	};

	template<class Ret, class obj, class F, class MMO(To)>
	struct meta_transfer<meta_ret_object<obj, F, Ret>, MMO(To)> {
		using type = typename MMO(To)::template meta_set<typename meta_ret_object<obj, F, Ret>::ret>;
	};

	template<class MMO(Obj1), class MMO(Obj2)>
	struct Meta_Object_Invoke { using type = meta_invoke< MMO(Obj1), typename MMO(Obj2)::type>; };
	template<class MMO(Obj1), class Obj2, class F, class Ret>
	struct Meta_Object_Invoke<MMO(Obj1), meta_ret_object<Obj2, F, Ret>>
	{ 
		using type = meta_invoke<MMO(Obj1), typename meta_ret_object<Obj2, F, Ret>::ret>; 
	};
	template<class MMO(Obj1), class MMO(Obj2)>
	using meta_object_invoke = typename Meta_Object_Invoke< MMO(Obj1), MMO(Obj2)>::type;




	//transfer obj between MO, Fn not included.
	template<class MMO(From), class MMO(To)>
	using meta_transfer_object = typename  meta_transfer<MMO(From), MMO(To)>::type;

	

	template<bool _continue, class MMO(Condition), class MMO(Obj), class MMO(Generator) = meta_object<meta_empty, meta_empty_fn>> struct meta_looper
	{
			
		//invoke from generator
		template<class ...Args> struct apply {

			//transfer current obj to  con_obj to judge
			using _continue_t = typename meta_invoke<meta_transfer_object<MMO(Obj), MMO(Condition)>>::type;
			static const bool _continue_ = _continue_t::value;
			using generator_stage_o = meta_invoke<MMO(Generator), Args...>;//invoke generator object;
			using result_stage_o = exp_if<_continue_, meta_object_invoke<MMO(Obj), generator_stage_o>, MMO(Obj)>;
			using next_stage = meta_looper<_continue_, MMO(Condition), result_stage_o, generator_stage_o>;
			using track_apply_t =typename meta_looper<
				_continue_, 
				MMO(Condition),
				result_stage_o,
				generator_stage_o
			>::template apply<Args...>;
			using type = typename track_apply_t::type;
		};
	};

	template<class Cond, class MO, class Generator> struct meta_looper<false, Cond, MO, Generator>
	{
		template<class ...Args> struct apply
		{
			using type = MO;
		};
	};
	template<class MMO(Condition), class MMO(Obj), class MMO(Generator) = meta_object<meta_empty, meta_empty_fn>>
	using meta_looper_t =typename meta_invoke<meta_looper<true, MMO(Condition), MMO(Obj), MMO(Generator)>>::type;

	template<class MMO(Condition), class MMO(Obj), class MMO(Generator) = meta_object<meta_empty, meta_empty_fn>>
	using meta_looper_stage = meta_invoke<meta_looper<true, MMO(Condition), MMO(Obj), MMO(Generator)>>;

	

	//common meta_object container
	namespace common_object 
	{
		struct append
		{
			template<class thisObj, class T> struct append_impl
			{
				using type = exp_list<thisObj, T>;
			};
			template<template<class...> class TL, class Outer_ty, class...Typs> struct append_impl <TL<Typs...>, Outer_ty>
			{
				using type = TL<Typs..., Outer_ty>;
			};
			template<class thisObj, class _2> using apply = typename append_impl<thisObj, _2>::type;
		};
		struct decreased
		{
			template<class TL, class...> using apply = typename split_first<TL>::rest;
		};
		template<size_t _limit> struct size_limiter
		{
			template<class TL, class...> struct apply
			{
				static const bool value = size_of_type_list<TL>::value < _limit;
			};
		};
		template<size_t _above> struct size_above
		{
			template<class TL> struct apply
			{
				static const bool value = (size_of_type_list<TL>::value > _above);
			};
		};
		struct auto_inc_gen
		{
			template<class _idx, class...> using apply = exp_repeat::inc_idx_t<_idx>;
		};
		struct timer_receiver {
			template<class timer,class...>
			struct apply :timer
			{};
		};
		struct decrease_ret
		{
			template<class TL> struct first_type {
				using type = void;
			};
			template<template<class...> class TL, class T, class...rest> struct first_type <TL<T, rest...>>
			{ 
				using type = T; 
			};
			template<class TL> using apply = exp_if<size_of_type_list<TL>::value != 0, 
				typename first_type<TL>::type, 
				TL
			>;
		};
		template<class condition_f> struct fliter : append
		{
			template<class thisObj, class _2> using apply = exp_if<
				(!meta_invoke<condition_f, thisObj, _2>::value),
				append::apply<thisObj, _2>,
				thisObj
			>;
		};
		struct replace_transform {
			template<class thisObj, class _2> using apply = _2;
		};
		//an mo replace to replace itself in looper
		using meta_replace_o = meta_object<meta_empty, replace_transform>;

		template<size_t N> using meta_replace_to = meta_timer_object<N, meta_empty, replace_transform>;
		//an mo convert typelist to an appendable list in looper
		template<class TL> using meta_appendable_o = meta_object<TL, append>;

		//an mo convert typelist to an appendable list with fliter in looper
		template<class TL, class fn> using meta_appendable_fliter_o = meta_object<TL, fliter<fn>>;

		//an mo convert typelist to an decreasible list in looper
		template<class TL> using meta_decreasible_o = meta_object<TL, decreased>;

		template<class TL> using meta_ret_decreasible_o = meta_ret_object<TL, decreased, decrease_ret>;

		//generate a mo which limit max size of typelist when invoke
		template<size_t N> using meta_length_limiter_o = meta_object<meta_empty, size_limiter<N>>;

		//generate a mo which limit min size of typelist when invoke
		template<size_t N> using meta_length_above_o = meta_object<meta_empty, size_above<N>>;
	    
		/*generate a mo which produces a greater idx when invoked*/
		template<size_t N> using meta_idx_c_go = meta_object<exp_repeat::Idx<N>, auto_inc_gen>;

		//generate a conditional type mo
		template<class Fn> using meta_condition_c_o = meta_object<meta_empty, Fn>; 

		//empty mo, mo container
		using meta_empty_o = meta_object<meta_empty, meta_empty_fn>; 

		using meta_timer_cnd_o = meta_object<meta_empty, timer_receiver>;
		template<class ...Typs> using decrease_list = exp_list<meta_empty, Typs...>;
	}
	template<class MMO(Obj), class MMO(Generator) = common_object::meta_empty_o>
	using meta_timer_looper_t = meta_looper_t<common_object::meta_timer_cnd_o, MMO(Obj), MMO(Generator)>;

	template<class _del_idx, class _ins_idx, class TL>
	struct delete_and_insert_to
	{
		template<class _idx> struct nth_of
		{
			static_assert(_idx::value <= max_type_list_index<TL>::value, "idx overflow!!!");
			template<class TL> using apply = exp_select<_idx::value, TL>;
		};
		template<class _idx> struct del_at
		{
			static_assert(_idx::value <= max_type_list_index<TL>::value, "idx overflow!!!");
			template<class TL> using apply = typename experiment::erase_at_t<_idx::value, TL>::type;
		};
		template<class _idx, class T> struct ins_at
		{
			static_assert(_idx::value <= max_type_list_index<TL>::value, "idx overflow!!!");
			template<class TL> using apply = insert_at<TL, inserter<_idx::value, T>>;
		};
		using recv = meta_object<meta_empty, nth_of<_del_idx>>;
		using delr = meta_object<TL, del_at<_del_idx>>;
		using recv_inv = meta_transfer_object<delr, recv>;
		using delr_t = typename meta_invoke<delr>::type;
		using recv_t = typename meta_invoke<recv_inv>::type;
		using type = meta_invoke<ins_at<_ins_idx, recv_t>, delr_t>;
	};

	template<class idx_start, class idx_end, class TL> struct meta_swap
	{
		static const bool start_is_greater = greater::template apply<idx_start, idx_end>::value;
		using stage_t = delete_and_insert_to<idx_start, idx_end, TL>::type;
		using type = delete_and_insert_to<
			exp_if<start_is_greater, inc_idx_t<idx_end>, meta_expr<idx_end, sub, exp_repeat::Idx<1>>>,
			idx_start, stage_t>::type;
	};
	template<class I> struct meta_integer_segment {};
	template<size_t I> struct meta_integer_segment<Idx<I>>
	{
		template<class T> struct count_impl {};
		template<size_t counts> requires (counts >= 1) struct count_impl<Idx<counts>>
		{
			using count_gen = typename meta_timer_looper_t<
				meta_timer_object<counts - 1, exp_list<>, common_object::append>,
				common_object::meta_idx_c_go<I>
			>::type;
			using type = get_type<add_to_front<Idx<I>, count_gen>>;
		};
		template<class counts>
		using apply = get_type<count_impl<counts>>;
	};

	template<size_t I, size_t counts> requires (counts >= 1)
		using meta_cout = meta_integer_segment<Idx<I>>::template apply<Idx<counts>>;

	template<size_t I, size_t counts> using list_slice = exp_repeat::meta_to_array<
		meta_cout<I, counts>>::template to<exp_select_list>;
}