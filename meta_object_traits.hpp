#pragma once
#include "exp_repeat.hpp"
#include <type_traits>


namespace meta_traits
{
#define quick_invoke(tmp, alias) typename tmp::template alias
#define quick_apply(tmp) quick_invoke(tmp, apply)
#define no_typename_apply(tmp) tmp::template apply
#define quick_value(T) T::value
#define MMO(t) m_meta_object_##t

	using namespace exp_repeat;
	using namespace exp_repeat::operators;
	

	template<class T> struct quick_reverse
	{
		static constexpr bool value = (!T::value);
	};

	template<size_t N> struct quick_value_i
	{
		static constexpr size_t value = N;
	};

	template<bool b> struct quick_value_b
	{
		static constexpr bool value = b;
	};

	template<class T> constexpr bool quick_bool_value = T::value;
	

	template<size_t A, size_t B> struct quick_value_i_greater
	{
		static constexpr bool value = (A > B);
	};
	template<size_t A, size_t B> struct quick_value_i_greater_equal
	{
		static constexpr bool value = (A >= B);
	};
	template<size_t A, size_t B> struct quick_value_i_lesser
	{
		static constexpr bool value = (A < B);
	};
	template<size_t A, size_t B> struct quick_value_i_lesser_equal
	{
		static constexpr bool value = (A <= B);
	};

	template<class TL1, class TL2> 
	struct quick_size_compare_greater : quick_value_i_greater<exp_size<TL1>, exp_size<TL2>> {};

	template<class TL1, class TL2> 
	struct quick_size_compare_greater_equal : quick_value_i_greater_equal<exp_size<TL1>, exp_size<TL2>> {};

	template<class TL1, class TL2> 
	struct quick_size_compare_lesser : quick_value_i_lesser<exp_size<TL1>, exp_size<TL2>> {};

	template<class TL1, class TL2> 
	struct quick_size_compare_lesser_equal : quick_value_i_lesser_equal<exp_size<TL1>, exp_size<TL2>> {};



	template<class F, class TL>
	struct exp_fn_apply_impl {};

	template<class F, template<class ...> class TL, class ...L>
	struct exp_fn_apply_impl<F, TL<L...>>
	{
		using type = TL<meta_invoke<F, L>...>;
	};

	template<class F, class TL> using exp_fn_apply = typename exp_fn_apply_impl<F, TL>::type;

	struct meta_empty { static constexpr int value = 0; };
	struct meta_empty_fn { template<class T, class ...> using apply = T;};

	struct meta_break_signal :std::false_type {};

	struct meta_always_continue
	{
		template<class T> struct apply :std::false_type
		{};
	};
	
	template<class Fn, class DF> struct meta_break_if
	{
		template<class T> using apply = exp_if<meta_invoke<Fn, T>::value, meta_break_signal, DF>;
	};


	
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

	template<size_t times, class MMO(Obj), class F, class break_f = meta_always_continue>
	struct meta_timer_object
	{
		using timer =meta_invoke<meta_break_if<break_f, quick_value_i_greater<times, 0>>, MMO(Obj)>;

		using type = MMO(Obj);
		template<class ...Arg>
		using apply = meta_timer_object<times - 1, meta_invoke<F, MMO(Obj), Arg...>, F, break_f>;

		template<class Outer_Obj>
		using meta_set = meta_timer_object<times, Outer_Obj, F, break_f>;

		template<size_t reset_time>
		using reset = meta_timer_object<reset_time, MMO(Obj), F, break_f>;
	};

	template<class MMO(Obj), size_t N, class break_f> struct To_Timer{};
	template<class obj, class F, size_t N, class break_f> struct To_Timer<meta_object<obj, F>, N, break_f>
	{
		using type = meta_timer_object<N, obj, F, break_f>;
	};

	template<class MMO(Obj), size_t N, class break_f = meta_always_continue> using to_timer = typename To_Timer<MMO(Obj), N, break_f>::type;

	template<class mo, class F> struct Break_If {};
	template<size_t N, class MMO(Obj), class MO_F, class F> struct Break_If<meta_timer_object<N, MMO(Obj), MO_F>, F>
	{
		using type = meta_timer_object<N, MMO(Obj), MO_F, F>;
	};

	template<class MTO, class BF> using break_if = get_type<Break_If<MTO, BF>>;

	template<class MMO(From), class MMO (To)>
	struct meta_transfer {
		using type = typename MMO(To)::template meta_set<typename MMO(From)::type>;
	};

	//transfer timer if invoke to a meta_timer_oject
	template<size_t times, class obj, class F, class MMO(To), class B>
	struct meta_transfer<meta_timer_object<times, obj, F, B>, MMO(To)> {
		using type = typename MMO(To)::template meta_set<typename meta_timer_object<times, obj, F, B>::timer>;
	};

	//transfer returns if invoke to a meta_ret_object
	template<class Ret, class obj, class F, class MMO(To)>
	struct meta_transfer<meta_ret_object<obj, F, Ret>, MMO(To)> {
		using type = typename MMO(To)::template meta_set<typename meta_ret_object<obj, F, Ret>::ret>;
	};

	//the meta_object is itself a meta_function
	//if two meta_objects invoked, invoke the first object with type in second object
	template<class MMO(Obj1), class MMO(Obj2)>
	struct Meta_Object_Invoke { using type = meta_invoke< MMO(Obj1), typename MMO(Obj2)::type>; };

	//if invoke with meta_ret_object, invoke the first object with returns
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

	//use meta_object_invoke to invoke two meta objects if true
	//meta object must be invoked with another meta object being argument
	//if false, return original meta object
	template<bool con> struct invoke_object_if
	{
		template<bool, class MO1, class MO2> struct meta_o_branch
		{
			using type = MO1;
		};
		template<class MO1, class MO2> struct meta_o_branch<true, MO1, MO2>
		{
			using type = meta_object_invoke<MO1, MO2>;
		};

		template<class MO1, class MO2> using apply = typename meta_o_branch<con, MO1, MO2>::type;
	};

	//using meta_invoke to invoke a meta function with arguments if true
	//if false, return the invoked meta function itself
	//this is designed for meta_object, because if it returns the function itself
	//it returns the object with not being invoked
	template<bool con> struct invoke_meta_function_if
	{
		template<bool, class F, class ...Args> struct meta_function_branch
		{
			using type = F;
		};
		template<class F, class ...Args> struct meta_function_branch<true, F, Args...>
		{
			using type = meta_invoke<F, Args...>;
		};

		template<class F, class ...Args> using apply = typename meta_function_branch<con, F, Args...>::type;
	};



	
	//Note: looper returns a meta object, not the context itself
	//Note: with the macro MMO, it requires the template class to be a meta_object
	template<bool, class MMO(Condition), class MMO(Obj), class MMO(Generator) = meta_object<meta_empty, meta_empty_fn>> struct meta_looper
	{
			
		
		template<class ...Args> struct apply {

			//transfer current obj to  condition_obj to judge
			//transfer different context based on types of meta_object
			using _continue_t = typename meta_invoke<meta_transfer_object<MMO(Obj), MMO(Condition)>>::type;
			static const bool _continue_ = _continue_t::value;

			//invoke generator object if condition is true
			using generator_stage_o = typename invoke_meta_function_if<_continue_>::template apply<MMO(Generator), Args...>;

			//invoke Obj object if condition is true
			using result_stage_o = typename invoke_object_if<_continue_>::template apply<MMO(Obj), generator_stage_o>;

			//for debug
			using next_stage = meta_looper<_continue_, MMO(Condition), result_stage_o, generator_stage_o>;

			//recursively loop for result
			using track_apply_t = meta_invoke<invoke_meta_function_if<_continue_>, meta_looper<
				_continue_, 
				MMO(Condition),
				result_stage_o,
				generator_stage_o
			>, Args...>;
			using type = typename track_apply_t::type;
		};
	};

	template<class Cond, class MO, class Generator> struct meta_looper<false, Cond, MO, Generator>
	{
		static constexpr bool _continue_ = false;
		using type = MO;
	};
	template<class MMO(Condition), class MMO(Obj), class MMO(Generator) = meta_object<meta_empty, meta_empty_fn>>
	using meta_looper_t =typename meta_invoke<meta_looper<true, MMO(Condition), MMO(Obj), MMO(Generator)>>::type;

	template<class MMO(Condition), class MMO(Obj), class MMO(Generator) = meta_object<meta_empty, meta_empty_fn>>
	using meta_looper_stage = meta_invoke<meta_looper<true, MMO(Condition), MMO(Obj), MMO(Generator)>>;

	//easy interface for template functor recursion
	//imagination of while constexpr

	template<class...> struct while_constexpr {};
	template<typename mo_cnd, typename mo_obj, typename mo_gen> 
	struct while_constexpr<mo_cnd, mo_obj, mo_gen>
	{
		template<class MO> struct is_breakable :std::false_type {};

		template<size_t N, class Obj, class MO_F, class BF>
		struct is_breakable<meta_timer_object<N, Obj, MO_F, BF>> : std::true_type {};

		using current_looper = meta_invoke<meta_looper<true, mo_cnd, mo_obj, mo_gen>>;

		template<typename F, typename ...Args>
		inline constexpr void recursively_invoke(F&& f, Args &&...args) const noexcept
		{
			
			if constexpr (current_looper::_continue_)
			{
				f.template operator() <typename mo_obj::type> (std::forward<Args>(args)...);
				while_constexpr<mo_cnd, typename current_looper::result_stage_o, typename current_looper::generator_stage_o>
				{}.recursively_invoke(std::forward<F>(f), std::forward<Args>(args)...);
			}
			else
			{
				f.template operator() < typename mo_obj::type > (std::forward<Args>(args)...);
			}
		}

		//provide a transform function to automatically use while_constexpr transform
		template<typename F, typename TF, class O, typename ...Args> 
		inline constexpr void recursively_transform_invoke(F&& f, TF&& tf, O&& o, Args&& ...args) const noexcept
		{
			if constexpr (current_looper::_continue_)
			{
				f.template operator()<typename mo_obj::type>(
					std::forward<O>(o),
					std::forward<Args>(args)...
					);
				while_constexpr<mo_cnd, typename current_looper::result_stage_o, typename current_looper::generator_stage_o>
				{}.recursively_transform_invoke(std::forward<F>(f), std::forward<TF>(tf), std::move(tf.template operator()(o)), std::forward<Args>(args)...);
			}
			else
			{
				f.template operator() < typename mo_obj::type > (
					std::forward<O>(o),
					std::forward<Args>(args)...
					);

			}
		}

		constexpr auto final_type() const noexcept-> typename current_looper::type
		{
			return {};
		}

		

		constexpr bool is_blocked() const noexcept
		{
			static_assert(is_breakable<mo_obj>::value, "Error:current meta_object is not a meta_timer_object! Only meta_timer_object is breakable!");

			return std::is_same_v<typename decltype(final_type())::timer, meta_break_signal> ;
		}
	};

	template<class mo_cnd, class mo_obj> 
	struct while_constexpr<mo_cnd, mo_obj> : while_constexpr<mo_cnd, mo_obj, meta_object<meta_empty, meta_empty_fn>>
	{};

	//common meta_object generator
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
			template<class T> struct first_type {
				using type = no_exist_type;
			};
			template<template<class...> class TL, class T, class...rest> struct first_type <TL<T, rest...>>
			{ 
				using type = T; 
			};
			template<class TL> using apply = typename first_type<TL>::type;
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

		template<size_t N> struct meta_value_limiter_i_f
		{
			template<class Val_t, class...> struct apply : quick_value_i_lesser<Val_t::value, N>
			{};
		};

		template<template<class ...> class apply_shape> struct meta_function_template_container {};

		template<class F, class En = void> struct is_meta_function_type : std::false_type {};

		template<class F> struct is_meta_function_type < F, std::void_t < meta_function_template_container<typename F::template apply >> > : std::true_type {};

		template<class F> constexpr bool is_meta_function_v = is_meta_function_type<F>::value;

		struct fold_apply_f
		{
			template<class ThisMo, class Fn> using apply = meta_invoke<invoke_meta_function_if<is_meta_function_v<Fn>>, Fn, ThisMo>;
		};
		
		struct node_forward_f
		{
			//if you'd like to exam if a node is available, use this_type::next_pointer_type instead
			//by that way you will iterate the node by the pointer, instead the element itself
			template<class this_type, class ...> using apply = typename this_type::next_type;
		};

		template<class Node> using node_forward_o = meta_object<Node, node_forward_f>;

		struct node_forward_condition
		{
			template<class this_type, class ...> struct apply
			{
				static constexpr bool value = this_type::has_next;
			};
		};

		

		template<size_t N> using meta_value_limiter_i_co = meta_object<Idx<0>, meta_value_limiter_i_f<N>>;

		//a mo to perform fold algorithm
		template<class T>
		using meta_fold_apply_o = meta_object<T, fold_apply_f>;

		//a mo replace that replaces itself in looper
		using meta_replace_o = meta_object<meta_empty, replace_transform>;

		//a timer version of replace_o
		template<size_t N> using meta_replace_to = meta_timer_object<N, meta_empty, replace_transform>;

		//a mo convert typelist to an appendable list in looper
		template<class TL> using meta_appendable_o = meta_object<TL, append>;

		//a mo convert typelist to an appendable list with filter in looper
		template<class TL, class fn> using meta_appendable_filter_o = meta_object<TL, fliter<fn>>;

		//a mo convert typelist to an decreasible list in looper
		template<class TL> using meta_decreasible_o = meta_object<TL, decreased>;

		//a mo decrease itself step by step, return what is decrease when invoked
		template<class TL> using meta_ret_decreasible_o = meta_ret_object<TL, decreased, decrease_ret>;

		//generate a condition mo which limit max size of typelist when invoked
		template<size_t N> using meta_length_limiter_o = meta_object<meta_empty, size_limiter<N>>;

		//generate a condition mo which limit min size of typelist when invoked
		template<size_t N> using meta_length_above_o = meta_object<meta_empty, size_above<N>>;
	    
		//a generator mo which produces a greater idx when invoked
		template<size_t N> using meta_idx_inc_gen_o = exp_if<
			N!=0, 
			meta_object<exp_repeat::Idx<N-1>, auto_inc_gen>,
			meta_object<exp_repeat::below_zero, auto_inc_gen>
			>;

		//generate a conditional type mo
		template<class Fn> using meta_condition_c_o = meta_object<meta_empty, Fn>; 

		//empty mo, mo container
		using meta_empty_o = meta_object<meta_empty, meta_empty_fn>; 

		//common timer condition mo 
		using meta_timer_cnd_o = meta_object<meta_empty, timer_receiver>;

		//a list for decrease mo incase of an empty list
		template<class ...Typs> using decrease_list = exp_list<meta_empty, Typs...>;

		template<class TL> using decrease_transform = quick_invoke(get_type<to_exp_list<TL>>, to)<decrease_list>;

		template<class...Typs> struct type_list_size<exp_list<meta_empty, Typs...>>
		{
			static constexpr size_t value = sizeof...(Typs);
		};
		//a condition mo the determin if a exp_node object is nextable
		using node_forward_condition_o = meta_condition_c_o<node_forward_condition>;
	}
	template<class MMO(Obj), class MMO(Generator) = common_object::meta_empty_o>
	using meta_timer_looper_t = meta_looper_t<common_object::meta_timer_cnd_o, MMO(Obj), MMO(Generator)>;

	template<size_t N, class mo_obj, class mo_f, class break_f>
	struct while_constexpr<meta_timer_object<N, mo_obj, mo_f, break_f>> :
		while_constexpr<common_object::meta_timer_cnd_o, meta_timer_object<N, mo_obj, mo_f, break_f>>
	{};

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
				common_object::meta_idx_inc_gen_o<I>
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