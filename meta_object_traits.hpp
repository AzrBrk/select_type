#pragma once
#include "exp_repeat.hpp"
#include <type_traits>
namespace meta_traits
{
#define __MO(t) _meta_object_##t
	using namespace exp_repeat;
	using namespace exp_repeat::operators;

	struct meta_empty {};
	struct meta_empty_fn { template<class T, class ...> using apply = T;};


	
	/*A meta obj is a bind of a meta_function and an obj, each time it is invoked, it update itself to a new type,
	use ::type to get the inner obj*/
	template<class __MO(Obj), class F/*Define how to Update an obj*/>
	struct meta_object
	{
		using type = __MO(Obj);
		template<class ...Arg>
		using apply = meta_object<meta_invoke<F, __MO(Obj), Arg...>, F>;

		template<class Outer_Obj>
		using meta_set = meta_object<Outer_Obj, F>;
	};

	template<class __MO(Obj), class F, class Ret>
	struct meta_ret_object :meta_object< __MO(Obj), F>
	{
		using ret = meta_invoke<Ret, __MO(Obj)>;
		using type = __MO(Obj);
		template<class ...Arg>
		using apply = meta_ret_object<meta_invoke<F, __MO(Obj), Arg...>, F, Ret>;

		template<class Outer_Obj>
		using meta_set = meta_ret_object<Outer_Obj, F, Ret>;
	};

	template<size_t _times, class __MO(Obj), class F>
	struct meta_timer_object
	{
		template<size_t ts> struct timer_
		{
			static const bool value = (ts > 0);
		};
		using timer = timer_<_times>;
		using type = __MO(Obj);
		template<class ...Arg>
		using apply = meta_timer_object<_times - 1, meta_invoke<F, __MO(Obj), Arg...>, F>;

		template<class Outer_Obj>
		using meta_set = meta_timer_object<_times, Outer_Obj, F>;
	};


	template<class __MO(From), class __MO(To)>
	struct meta_transfer {
		using type = typename __MO(To)::template meta_set<typename __MO(From)::type>;
	};

	template<size_t times, class obj, class F, class __MO(To)>
	struct meta_transfer<meta_timer_object<times, obj, F>, __MO(To)> {
		using type = typename __MO(To)::template meta_set<typename meta_timer_object<times, obj, F>::timer>;
	};
	template<class Ret, class obj, class F, class __MO(To)>
	struct meta_transfer<meta_ret_object<obj, F, Ret>, __MO(To)> {
		using type = typename __MO(To)::template meta_set<typename meta_ret_object<obj, F, Ret>::ret>;
	};

	template<class __MO(Obj1), class __MO(Obj2)>
	struct Meta_Object_Invoke { using type = meta_invoke< __MO(Obj1), typename __MO(Obj2)::type>; };
	template<class __MO(Obj1), class Obj2, class F, class Ret>
	struct Meta_Object_Invoke<__MO(Obj1), meta_ret_object<Obj2, F, Ret>>
	{ 
		using type = meta_invoke<__MO(Obj1), typename meta_ret_object<Obj2, F, Ret>::ret>; 
	};
	template<class __MO(Obj1), class __MO(Obj2)>
	using meta_object_invoke = typename Meta_Object_Invoke< __MO(Obj1), __MO(Obj2)>::type;



	//transfer obj between MO, Fn not included.
	template<class __MO(From), class __MO(To)>
	using meta_transfer_object = typename  meta_transfer<__MO(From), __MO(To)>::type;

	

	template<bool _continue, class __MO(Condition), class __MO(Obj), class __MO(Generator) = meta_object<meta_empty, meta_empty_fn>> struct meta_looper
	{
		//transfer current obj to  con_obj to judge
		using _continue_t = typename meta_invoke<meta_transfer_object<__MO(Obj), __MO(Condition)>>::type;
		static const bool _continue_ = _continue_t::value;
			
		//invoke from generator
		template<class ...Args> struct apply {
			using _transform_stage_o = meta_invoke<__MO(Generator), Args...>;//invoke generator object;
			using track_apply_t =typename meta_looper<
				_continue_, __MO(Condition),
				exp_if<_continue_, meta_object_invoke<__MO(Obj), _transform_stage_o>, __MO(Obj)>,//if condition is false, the transform should
																										  //stop instead of continuing the transformation
				_transform_stage_o
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
	template<class __MO(Condition), class __MO(Obj), class __MO(Generator) = meta_object<meta_empty, meta_empty_fn>>
	using meta_looper_t =typename meta_invoke<meta_looper<true, __MO(Condition), __MO(Obj), __MO(Generator)>>::type;
	//common meta_object container
	namespace common_object 
	{
		struct append
		{
			template<class _1, class _2> using apply = exp_join_impl<_1, _2>::type;
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
		//an mo convert typelist to an appendable list in looper
		template<class TL> using meta_appendable_o = meta_object<TL, append>;
		//an mo convert typelist to an decreasible list in looper
		template<class TL> using meta_decreasible_o = meta_object<TL, decreased>;
		template<class TL> using meta_ret_decreasible_o = meta_ret_object<TL, decreased, decrease_ret>;
		//generator a mo which limit max size of typelist when invoke
		template<size_t N> using meta_length_limiter_o = meta_object<meta_empty, size_limiter<N>>;
		//generator a mo which limit min size of typelist when invoke
		template<size_t N> using meta_length_above_o = meta_object<meta_empty, size_above<N>>;
	    
		/*generator a mo which produces a greater idx when invoked*/
		template<size_t N> using meta_idx_c_go = meta_object<exp_repeat::Idx<N>, auto_inc_gen>;
		//generator a conditional type mo
		template<class Fn> using meta_condition_c_o = meta_object<meta_empty, Fn>; 
		//empty mo, mo container
		using meta_empty_o = meta_object<meta_empty, meta_empty_fn>; 

		using meta_timer_cnd_o = meta_object<meta_empty, timer_receiver>;
		template<class ...Typs> using decrease_list = exp_list<meta_empty, Typs...>;
	}

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

	template<class _idx_start, class _idx_end, class TL> struct meta_swap
	{
		static const bool _start_is_greater = greater::template apply<_idx_start, _idx_end>::value;
		using _stage_t = delete_and_insert_to<_idx_start, _idx_end, TL>::type;
		using type = delete_and_insert_to<
			exp_if<_start_is_greater, inc_idx_t<_idx_end>, meta_expr<_idx_end, sub, exp_repeat::Idx<1>>>,
			_idx_start, _stage_t>::type;
	};
	
	template<class T, size_t N> struct meta_types_c
	{
		using result = typename meta_looper<true, 
			common_object::meta_length_limiter_o<N>, 
			common_object::meta_appendable_o<exp_list<>>, 
			common_object::meta_idx_c_go<0>
		>::template apply<>::type;

		using type = typename result::type;
	};
	template<class T> struct meta_types_c<T, 0>//Éú³ÉÊ²Ã´£¿
	{
		using type = std::false_type;
	};
	namespace sort {
		struct compare_1st_and_2nd
		{
			template<class TL> struct available
			{
				using cmp_tl = exp_select_list<0, 1>::template apply<TL>;
				static const bool value = !(exp_repeat::operators::greater::apply<exp_select<0, cmp_tl>, exp_select<1, cmp_tl>>::value);
			};
			template<class TL> struct unavailable
			{
				static const bool value = true;
			};
			template<class TL> struct apply
			{
				static const bool value = exp_if<(size_of_type_list<TL>::value >= 2), available<TL>, unavailable<TL>>::value;
			};
		};
		template<bool _status, size_t _size> struct cmp_o
		{
			static const bool value = _status;
			static const size_t size = _size;
		};

		struct cmp_f
		{
			template<class TL> struct status_gen
			{
				using type = cmp_o<compare_1st_and_2nd::apply<TL>::value, size_of_type_list<TL>::value>;
			};
			template<class mo, class TL, class...> using apply = typename status_gen<TL>::type;

		};

		struct cnd_f
		{
			template<class ob> struct apply
			{
				static const bool value = (ob::size >= 2 && ob::value);
			};
		};

		using cmp_mo = meta_object<cmp_o<true, 4>, cmp_f>;
		using cnd_mo = meta_object<meta_empty, cnd_f>;

		template<class TL>
		using is_less_ordered_sorted_array = meta_looper<true, cnd_mo, cmp_mo, common_object::meta_decreasible_o<TL>>::template apply<>::type;

		template<class TL>
		constexpr bool is_less_ordered_sorted_v = is_less_ordered_sorted_array<TL>::type::value;

		template<class TL>
		using unordered_info =typename is_less_ordered_sorted_array<TL>::type;

		struct cnd_is_less_ordered_array_o
		{
			template<class TL> struct apply { static const bool value = !is_less_ordered_sorted_v<TL>; };
		};

		struct sort_array
		{
			template<class TL> struct apply_impl {
				static const size_t unordered_pos = size_of_type_list<TL>::value - unordered_info<TL>::size;
				using type = typename meta_swap<Idx<unordered_pos>, Idx<unordered_pos+1>, TL>::type;
			};
			template<class TL> struct unavilable { using type = TL; };

			template<class TL, class...>
			using apply = typename exp_if <(unordered_info<TL>::size >= 2),apply_impl<TL>, unavilable<TL>>::type;
		};
		template<class TL>
		using sort_mo = meta_object<TL, sort_array>;

		using cnd_ordered_mo = common_object::meta_condition_c_o<cnd_is_less_ordered_array_o>;

		//explain: 
		//for(;cnd_ordered_mo return true; sort_mo<TL> = invoke_meta<sort_mo<TL>/*transform TL*/)
		//{
		//	transfer TL from sort_mo to cnd_ordered_mo;
		//	meta_invoke cnd_ordered_mo;
		// }
		template<class TL>
		using less_ordered_sort = meta_looper<!is_less_ordered_sorted_v<TL>, 
									cnd_ordered_mo, sort_mo<TL>,
								common_object::meta_empty_o>;
		template<class TL>
		struct type_index_c
		{
			struct generate_f
			{
				template<class tic, class idx> struct generate_t
				{
					using _1 = exp_select<0, tic>;//typelist to insert
					using _2 = exp_select<1, tic>;
					using combine_t = exp_list<idx, exp_select<0, typename _2::type>>;
					using type = exp_list<meta_invoke<_1, combine_t>, meta_invoke<_2>>;
				};
				template<class tic, class idx> using apply = typename generate_t<tic, idx>::type;
			};
			using looper_mo = meta_timer_object <
				max_type_list_index<TL>::value + 1,
				exp_list<common_object::meta_appendable_o<exp_list<>>,
				common_object::meta_decreasible_o<typename exp_join_impl<TL, void>::type>>,
				generate_f
				>;

			using res = meta_looper_t<common_object::meta_timer_cnd_o, looper_mo, common_object::meta_idx_c_go<0>>::type;
			using type = typename exp_select<0, res>::type;
		};


	}
}