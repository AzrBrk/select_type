#pragma once

#include"meta_object_traits.hpp"



namespace meta_typelist
{
	using namespace::meta_traits;
	using namespace::meta_traits::common_object;
	template<class MMO(To), class MMO(From)> struct meta_stream
	{
		using from =MMO(From);
		using to =MMO(To);
		using cache = typename MMO(From)::ret;
		using update = meta_stream<meta_object_invoke<MMO(To), MMO(From)>, meta_invoke<MMO(From)>>;
		template<class F> using apply = meta_invoke<F, MMO(To), MMO(From)>;
	};
	struct meta_stream_f
	{
		template<class mo_stream, class...>
		using apply = typename mo_stream::update;
	};

	template<class TL>
	using op_stream = meta_appendable_o<TL>;
	template<class TL>
	using ip_stream = meta_ret_decreasible_o<TL>;

	template<size_t Transfer_Length, class MMO(To), class MMO(From)> 
		requires (size_of_type_list<typename MMO(From)::type>::value >= Transfer_Length)
		using meta_stream_o = meta_timer_object<
		Transfer_Length,
		meta_stream< MMO(To), MMO(From)>,
		meta_stream_f
		>;
	template<class MMO(To), class MMO(From)>
	using meta_all_transfer = typename meta_timer_looper_t<
		meta_stream_o<size_of_type_list<typename MMO(From)::type>::value, MMO(To), MMO(From)>
	>::type;

	struct meta_linker
	{
		template<class MMO(op), class MMO(ip), class F> struct apply
		{
			using op = meta_invoke<MMO(op), meta_invoke<F, typename MMO(ip)::ret>>;
			using ip = meta_invoke<MMO(ip)>;
			using type =typename meta_all_transfer<op, ip>::to::type;
		};
	};

	template<class MMO(stream)> struct Meta_Stream_Transfer {};
	template<size_t Transfer_Length, class MMO(To), class MMO(From)>
	struct Meta_Stream_Transfer<meta_stream_o<Transfer_Length, MMO(To), MMO(From) >>
	{
		using mo = typename meta_timer_looper_t<meta_stream_o<Transfer_Length, MMO(To), MMO(From)>>::type;
		using type = typename meta_timer_looper_t<meta_stream_o<Transfer_Length, MMO(To), MMO(From)>>::type::to::type;
	};

	template<class MMO(stream)> using meta_stream_transfer = typename Meta_Stream_Transfer<MMO(stream)>::type;
	template<class MMO(stream)> using meta_stream_transfer_mo = typename Meta_Stream_Transfer<MMO(stream)>::mo;

	
	

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
			meta_stream_o<N, op_stream<meta_clear_t<TL>>,ip_stream<TL>>
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

			using erase =typename experiment::erase_at_t<N, selectable_list<Typs...>>::type;

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
	
}