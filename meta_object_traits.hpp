#pragma once
#include "exp_repeat.hpp"
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

	

	//transfer obj between MO, Fn not included.
	template<class __MO(From), class __MO(To)>
	using meta_transfer_object = typename __MO(To)::template meta_set<typename __MO(From)::type>;

	template<bool _continue, class __MO(Condition), class __MO(Obj), class __MO(Generator)> struct meta_looper
	{
		//transfer current obj to  con_obj to judge
		using _continue_t = typename meta_invoke<meta_transfer_object<__MO(Obj), __MO(Condition)>>::type;
		static const bool _continue_ = _continue_t::value;
			
		//invoke from generator
		template<class ...Args> struct apply {
			using _transform_stage_t = meta_invoke<__MO(Generator), Args...>;//invoke condition object;
			using track_apply_t =typename meta_looper<
				_continue_, __MO(Condition),
				exp_if<_continue_, meta_invoke<__MO(Obj), typename _transform_stage_t::type>, __MO(Obj)>,//if condition is false, the transform should
																										  //stop instead of continuing the transformation
				_transform_stage_t
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
		//an mo convert typelist to an appendable list in looper
		template<class TL> using meta_appendable_o = meta_object<TL, append>;
		//an mo convert typelist to an decreasible list in looper
		template<class TL> using meta_decreasible_o = meta_object<TL, decreased>;
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
}