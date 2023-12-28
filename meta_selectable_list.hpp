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
	template<size_t N> using meta_integer_istream = meta_ret_decreasible_o<typename meta_itoa<N>::template to<decrease_list>>;

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
		template<class mso, class ...Args> using apply = meta_invoke<F, typename mso::to::type, Args...>;
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

			//using erase =typename experiment::erase_at_t<N, selectable_list<Typs...>>::type;

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

	
}