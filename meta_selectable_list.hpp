#pragma once

#include"meta_object_traits.hpp"
#include"alias_arguments_count.hpp"



namespace meta_typelist
{
	using namespace meta_traits;
	using namespace meta_traits::common_object;
	

	//define meta_stream object
	//meta stream contains an input stream (From) and an output stream (To)
	template<class MMO(To), class MMO(From)> struct meta_stream
	{
		using from =MMO(From);
		using to =MMO(To);
		using cache = typename MMO(From)::ret;
		using update = meta_stream<
			typename invoke_object_if<(exp_size<typename MMO(From)::type> > 0)>
			::template apply<MMO(To), MMO(From)>, meta_invoke<MMO(From)>
		>;
	};
	struct meta_stream_f
	{
		template<class mo_stream, class...>
		using apply = typename mo_stream::update;
	};

	//common output stream
	template<class TL>
	using meta_ostream = meta_appendable_o<TL>;

	//common input stream
	template<class TL>
	using meta_istream = meta_ret_decreasible_o<TL>;

	//a stream that generate increasing integer
	template<size_t N> using meta_integer_istream = meta_ret_decreasible_o<typename meta_iota<N>::template to<decrease_list>>;

	//the meta_stream_object is based on meta_timer_object
	//add a break condition if you try to block the stream
	template<size_t Transfer_Length, class MMO(To), class MMO(From), class break_f = meta_always_continue> 
		//requires (size_of_type_list<typename MMO(From)::type>::value >= Transfer_Length)
		using meta_stream_o = meta_timer_object<
		Transfer_Length,
		meta_stream< MMO(To), MMO(From)>,
		meta_stream_f,
		break_f
		>;
	//do all transfer between two meta_object
	//add a break condition if you try to block the stream
	template<class MMO(To), class MMO(From), class break_f = meta_always_continue>
	using meta_all_transfer = typename meta_timer_looper_t<
		meta_stream_o<size_of_type_list<typename MMO(From)::type>::value, MMO(To), MMO(From), break_f>
	>::type;

	//a stream that will tranfer all types in From to To 
	template<class MMO(To), class MMO(From), class break_f = meta_always_continue> using meta_all_transfer_o = meta_stream_o<size_of_type_list<typename MMO(From)::type>::value, MMO(To), MMO(From), break_f>;

	

	template<class MMO(stream)> struct Meta_Stream_Transfer {};
	template<size_t Transfer_Length, class MMO(To), class MMO(From)>
	struct Meta_Stream_Transfer<meta_stream_o<Transfer_Length, MMO(To), MMO(From) >>
	{
		using timer = meta_timer_looper_t<meta_stream_o<Transfer_Length, MMO(To), MMO(From)>>;
		using mo = typename meta_timer_looper_t<meta_stream_o<Transfer_Length, MMO(To), MMO(From)>>::type;
		using type = typename meta_timer_looper_t<meta_stream_o<Transfer_Length, MMO(To), MMO(From)>>::type::to::type;
	};

	template<class MMO(stream)> using meta_stream_transfer = typename Meta_Stream_Transfer<MMO(stream)>::type;
	template<class MMO(stream)> using meta_stream_transfer_mo = typename Meta_Stream_Transfer<MMO(stream)>::mo;
	template<class MMO(stream)> using meta_stream_transfer_timer = typename Meta_Stream_Transfer<MMO(stream)>::timer;

	
	struct meta_linker
	{
		template<class MMO(op), class MMO(ip), class F> struct apply
		{
			using op = meta_invoke<MMO(op), meta_invoke<F, typename MMO(ip)::ret>>;
			using ip = meta_invoke<MMO(ip)>;
			using type = typename meta_all_transfer<op, ip>::to::type;
		};
	};

	template<class ...Typs>
	struct selectable_list;
	template<class TL>
	struct meta_clear{};
	template<template<class...> class TL, class...L>
	struct meta_clear<TL<L...>> { using type = TL<>; };
	template<class TL> using meta_clear_t = typename meta_clear<TL>::type;

	template<size_t N> struct meta_spliter
	{
		template<class TL>
		using transfer_s = typename meta_timer_looper_t<
			meta_stream_o<N, meta_ostream<meta_clear_t<TL>>,meta_istream<TL>>
		>::type;
		template<class TL>
		using back = typename transfer_s<TL>::from::type;
		template<class TL>
		using front = typename transfer_s<TL>::to::type;
		template<class TL, class F>
		using apply = meta_linker::apply<typename transfer_s<TL>::to, typename transfer_s<TL>::from, F>;
	};
	template<size_t N> struct meta_transform_at
	{
		template<class TL, class F> using apply = meta_spliter<N>::template transform<TL, F>;
	};


	template<size_t N, class T> struct meta_tag { 
		using type = T; 
		static const size_t value = N;
	};

	//quickly create quoted-meta-functions from the unquoted ones
	namespace quick_meta
	{
		using namespace alias_c;

		template<template<class ...> class al_f, class ...Args> struct invoke_alias
		{
			//this invoker calculate the needed arguments from the alias function and automatically apply it
			using real_args_list_so = meta_stream_o<alias_argc<al_f>(), meta_ostream<exp_list<>>, meta_istream<exp_list<Args...>>>;
			using type = quick_invoke(meta_stream_transfer<real_args_list_so>,to)<al_f>;
		};

		template<template<class...> class unquoted_meta_fn> struct quick_construct
		{
			template<class ...Args> using apply = get_type<invoke_alias<unquoted_meta_fn, Args...>>;
		};

		template<class T> struct quick_nested
		{
			template<class Unused = void> using apply = T;
		};

		template<template<class...> class unquoted_meta_fn, class ...Bounded_Fn_Args> struct quick_construct_bind
		{
			template<class ...Args> using apply = get_type<invoke_alias<unquoted_meta_fn, Bounded_Fn_Args..., Args...>>;
		};

		template<template<class...> class unquoted_meta_fn, template<class ...> class args_transform, class ...Bounded_Fn_Args>
		struct quick_construct_bind_transform_args
		{
			template<class ...Args> using apply = get_type<invoke_alias<unquoted_meta_fn, Bounded_Fn_Args..., args_transform<Args>...>>;;
		};

		template<template<class> class F> struct quick_meta_object_function_single_to_object
		{
			template<class _1, class ...> using apply = F<_1>;
		};

		template<template<class, class> class F> struct quick_meta_object_function
		{
			template<class _1, class _2> struct apply : F<_1, _2>{};
		};

		template<template<class, class> class F, class _2> struct quick_meta_object_function_bind
		{
			template<class this_obj, class ...> struct apply : F<this_obj, _2>
			{};
		};

		template<template<class, class> class F, class T> struct quick_meta_object_function_ignored_this
		{
			template<class this_obj, class mo_arg> struct apply :F<T, mo_arg> {};
		};

		
	}
	
	template<class mso> using stream_final_t = typename mso::to::type;
	template<class F> struct meta_stream_op
	{
		template<class mso, class ...Args> using apply = meta_invoke<F, stream_final_t<mso>, Args...>;
	};
	
	template<class ...Typs>
	struct selectable_list : exp_list<Typs...>
	{
		template<class replace_type> struct replace_impl
		{
			template<class T> using apply = replace_type;
		};
		struct replace_obj
		{
			template<class thisObj, class T> using apply = T;
		};
		template<size_t N> using replace_this_o = meta_timer_object<N, meta_empty_o, replace_obj>;

		template<size_t N> using get = typename meta_looper_t<
			meta_timer_cnd_o,
			replace_this_o<N + 1>,
			meta_ret_decreasible_o<decrease_list<Typs...>>>::type;

		template<size_t N>
		struct _tag {
			template<class T> using apply = meta_tag<N, T>;
		};

		template<size_t N> struct invoke
		{
			template<class T> 
			using replace_with = typename meta_spliter<N>::template apply<selectable_list<Typs...>, replace_impl<T>>::type;

			template<class F> 
			using transform_to = typename meta_spliter<N>::template apply<selectable_list<Typs...>, F>::type;
			
			using tag =typename transform_to<_tag<N>>;

			template<size_t NS> using tag_to = transform_to<_tag<NS>>;


			template<size_t idx>
			using swap_with = typename meta_swap<exp_repeat::Idx<N>, exp_repeat::Idx<idx>, selectable_list<Typs...>>::type;

			template<class T> 
			using insert = insert_at<selectable_list<Typs...>, inserter<N, T>>;
		};
	};

	template<> struct selectable_list<> {};

	template<class T> struct to_selectable { using type = selectable_list<T>; };

	template<template<class...> class TL, class ...Typs>
	struct to_selectable<TL<Typs...>> { using type = selectable_list<Typs...>; };

	template<template<class...> class TL>
	struct to_selectable<TL<>> { using type = selectable_list<>; };

	template<class TL>
	using to_selectable_t = typename to_selectable<TL>::type;

	template<class TL> struct meta_rename{};
	template<template<class...> class TL, class ...T> struct meta_rename<TL<T...>>
	{
		template<template<class...> class TL2>
		using with = TL2<T...>;
	};

	//example of using meta stream and meta object to generate a typelist with each type in typelist is uniqued
	//Note : a meta stream is itself an meta object
	struct unique_type_list
	{
		template<class TL, class T> struct is_one_of_f :std::false_type
		{};

		template<template<class...> class TL, class T> struct is_one_of_f<TL<>, T> :std::true_type
		{};

		template<template<class...> class TL, class T, class TO, class ...Args> struct is_one_of_f<TL<T, Args...>, TO>
		{
			static constexpr bool value = !exp_is_one_of<TO, TL<T, Args...>>::value;
		};

		template<class ThisMo, class T> struct apply
		{
			static constexpr bool value = !(is_one_of_f<ThisMo, T>::value);
		};
	};

	using unique_type_list_o = common_object::meta_appendable_filter_o<exp_list<>, unique_type_list>;

	template<class TL> using make_unique_list = typename meta_all_transfer<unique_type_list_o, meta_istream<TL>>::to::type;

	template<class TL, class MArr> struct meta_swap2_type
	{
		using iter = to_selectable_t<TL>;
		static constexpr size_t pos1 = MArr::at<0>::value;
		static constexpr size_t pos2 = MArr::at<1>::value;

		using first_transformed = typename iter::template invoke<pos1>::template replace_with<exp_select<pos2, TL>>;
		using type = typename first_transformed::template invoke<pos2>::template replace_with<exp_select<pos1, TL>>;

	};

	namespace list_common_object
	{
		template<class TL1, class TL2> struct append_list;
		template<template<typename...> typename tlist1, typename ...Typs1, template<typename ...> typename tlist2, typename ...Typs2>
		struct append_list<tlist1<Typs1...>, tlist2<Typs2...>>
		{
			using type = tlist1<Typs1..., Typs2...>;
		};

		template<class front, class back> struct trim_back { using type = no_exist_type; };
		template<
			template<class...> class front,
			template<class...> class back,
			class ...front_types,
			class back_first, class ...back_rest>
		struct trim_back<front<front_types...>, back<back_first, back_rest...>>
		{
			using type = typename trim_back<front<front_types..., back_first>, back<back_rest...>>::type;
		};
		template<template<class ...> class front,
			template<class> class back,
			class ...front_types,
			class back_type
		>
		struct trim_back<front<front_types...>, back<back_type>>
		{
			using type = front<front_types...>;
		};

		template<class T> struct trim;
		template<template<class...> class tlist, class ...types> struct trim<tlist<types...>>
		{
			using type = typename trim_back<tlist<>, tlist<types...>>::type;
		};

		template<class T> struct trim_f;
		template<template<class...> class tlist, class first, class ...rest> struct trim_f<tlist<first, rest...>>
		{
			using type = tlist<rest...>;
		};

		template<class...Args> struct meta_list :exp_list<Args...>
		{
			static constexpr std::size_t length = sizeof...(Args);

			template<template<typename ...> typename m_alias>
			using as = m_alias<Args...>;

			template<class T> using append = meta_list<Args..., T>;
			template<class T> using r_append = meta_list<T, Args...>;

			template<class T = void>
			using trim_last = get_type<trim<meta_list<Args...>>>;

			template<class T = void>
			using trim_front = get_type<trim_f<meta_list<Args...>>>;

		};
		template<class typelist> struct offset_array {};
		template<template<class...> class typelist, class ...types> struct offset_array<typelist<types...>>
		{
			using type = meta_array<sizeof(types)...>;
		};

		template<class typelist> using offset_array_t = get_type<offset_array<typelist>>;

		template<class TL> using to_meta_list_t = typename to_exp_list<TL>::type::template to<meta_list>;



		//trim typelist until meet con

		//from -> to
		struct reverse_decrease
		{
			template<class meta_tl> struct apply_impl
			{
				using type = no_exist_type;

			};

			template<template<class...> class this_tl, class first, class ...types> struct apply_impl<this_tl<first, types...>>
			{
				using type = typename meta_list<first, types...>::template trim_last<>::template to<this_tl>;
			};

			template<class this_tl, class ...> using apply = get_type<apply_impl<this_tl>>;
		};

		struct reverse_decrease_ret
		{
			template<class this_tl> struct apply_impl
			{
				using type = no_exist_type;
			};
			template<template<class...> class this_tl, class first, class ...types> struct apply_impl<this_tl<first, types...>>
			{
				using type = exp_select<
					max_index<this_tl<first, types...>>,
					this_tl<first, types...>
				>;
			};

			template<class this_tl> using apply = get_type<apply_impl<this_tl>>;
		};

		template<class TL> using meta_reverse_istream = meta_ret_object<TL, reverse_decrease, reverse_decrease_ret>;
		template<class TL> struct reverse_typelist
		{
			using type = typename meta_all_transfer<
				meta_ostream<exp_list<>>,
				meta_reverse_istream<TL>
			>::to::type;
		};

		template<class TL> using reverse_t = typename reverse_typelist<TL>::type;

		template<class TL> using meta_reverse_decrease_o = meta_object<TL, reverse_decrease>;
		template<class T> using meta_empty_alias = T;
		template<class T> using meta_self_o = meta_object<T, meta_empty_fn>;

		template<template<class> class F>
		struct list_apply_f
		{
			template<class this_tl, class T> using apply = exp_fn_apply<F<T>, this_tl>;
		};

		struct attach_f
		{
			template<class this_selectable_o, class T> using apply = selectable_list<typename this_selectable_o::template get<0>, T>;
		};
		template<class T> using meta_attach_o = meta_object<selectable_list<T, T>, attach_f>;


		template<class TL, template<class> class F> using meta_list_apply_o = meta_object<TL, list_apply_f<F>>;
		template<std::size_t N, class TL, template<class> class F> using meta_list_apply_to = meta_timer_object<N, TL, list_apply_f<F>>;


	}


	namespace collector
	{
		template<class looper> constexpr bool meta_looper_status = 
			meta_invoke<invoke_meta_function_if<is_meta_function_v<looper>>, looper>::_continue_;

		//break_condition
		struct collect_stream_break_if_looper_stop
		{
			template<class collect_stream> struct apply
			{
				static constexpr bool value = !meta_looper_status<typename collect_stream::from::type>;
			};
		};

		//progressing the looper, use the looper itself as a meta istream
		struct collect_stream_update_f
		{
			template<class this_looper> struct impl
			{
				using type = no_exist_type;
			};
			template<bool c, class Mc, class Mo, class Mg> struct impl<meta_looper<c, Mc, Mo, Mg>>
			{
				using l_invoke_t = meta_invoke<meta_looper<c, Mc, Mo, Mg>>;
				using type = meta_looper<c, Mc, 
					typename l_invoke_t::result_stage_o, typename l_invoke_t::generator_stage_o>;
			};
			template<class this_looper, class ...> using apply = typename impl<this_looper>::type;
		};

		struct collect_stream_ret_f
		{
			template<class this_looper> using apply = typename meta_invoke<this_looper>::result_stage_o::type;
		};

		//instantiate the type_list_size with looper type to make the looper size infinitely long
		//because we can't predict what is processing in the looper, we use a break condition instead to end the meta-stream
		template<bool c, class Mc, class Mo, class Mg>
		struct type_list_size<meta_looper<c, Mc, Mo, Mg>> :quick_value_i<static_cast<std::size_t>(-1)> {};

		//the meta-stream object can be use in a while_constexpr or meta_looper directly
		template<class looper> using meta_collect_ostream = break_if<meta_stream_o<
			exp_size<looper>,
			meta_ostream<exp_list<>>,
			meta_ret_object<looper, collect_stream_update_f, collect_stream_ret_f>
		>, collect_stream_break_if_looper_stop>;

		template<class looper> struct looper_collector
		{
			using type = typename meta_timer_looper_t<meta_collect_ostream<looper>>::type::to::type;
		};
		template<class looper> using collect = typename looper_collector<looper>::type;
		template<std::size_t N, class os, class ins>
		struct collector::looper_collector<meta_stream_o<N, os, ins>> :
			exp_apply<typename collector::looper_collector<make_timer_loop<meta_stream_o<N, os, ins>>>::type, stream_final_t>
		{};


		template<class T, template<class> class ...collectable_f> struct meta_pipe_collect_stream_impl
		{
			template<class obj, template<class ...> class ...collectable_> struct collect_apply_recurse;
			template<class obj, template<class...> class first, template<class...> class ...rest> struct collect_apply_recurse<obj, first, rest...>
			{
				using recurse_type = collect_apply_recurse<collect<first<obj>>, rest...>;
				using type = typename recurse_type::type;
			};
			template<class obj> struct collect_apply_recurse<obj>
			{
				using type = obj;
			};
			using type = typename collect_apply_recurse<T, collectable_f...>::type;
		};

		template<class T, template<class> class...collectable_f> using meta_pipe = typename meta_pipe_collect_stream_impl<T, collectable_f...>::type;
	}
	template<class looper, class F, class ...Args>
	constexpr std::size_t collect_looper(F&& f, Args&& ...args)
	{
		using used_types = collector::collect<looper>;
		return template_func_execute_launcher(used_types{}, f, std::forward<Args>(args)...);
	}
}