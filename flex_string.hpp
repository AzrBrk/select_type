#pragma once

#include"exp_vh_node.hpp"
#include<format>
#define meta_while meta_looper_t

namespace flex_string
{
	template<class T>
	concept formatible = requires(T x) { std::format("{}", x); };

	template<class T> struct is_formatible { static const bool value = formatible<T>; };
	template<class ...Typs> constexpr bool check_formatible_v = std::conjunction_v<is_formatible<Typs>...>;

	using namespace meta_typelist;
	template<char c> struct char_val { static constexpr char value = c; };

	template<class ...char_vals> struct chars : exp_list<char_vals...> {
		using chars_to_selectable = selectable_list<char_vals...>;
	};

	template<char ...cs> using exp_chars = chars<char_val<cs>...>;
	template<char c> using exp_char = char_val<c>;

	using lbracket = char_val<'{'>;
	using rbracket = char_val<'}'>;

	struct app_bracket_f
	{
		template<class thisMo> struct app_bracket_impl {
			using add_lbracket = common_object::append::template apply<thisMo, lbracket>;
			using add_rbracket = common_object::append::template apply<add_lbracket, rbracket>;
		};
		template<class thisMo, class...>
		using apply = typename app_bracket_impl<thisMo>::add_rbracket;
	};
	template<size_t Times>
	using app_bracket_mo = meta_timer_object<Times, chars<>, app_bracket_f>;

	template<size_t Times>
	using generate_brackets = typename meta_timer_looper_t<app_bracket_mo<Times>>::type;

	namespace meta_string_stream
	{
		using namespace meta_typelist;
		template<bool cond> struct meta_if
		{
			template<class Ty1, class Ty2>
			using apply = std::_Select<cond>::_Apply<Ty1, Ty2>;
		};
		namespace repeator
		{
			template<class...TS> struct repeat_list :exp_list<TS...> {};
			template<class...TS> struct repeat_mode :exp_list<TS...> {};
			template<class T> using generate_self_o = meta_object<T, meta_empty_fn>;
			template<size_t N, class T> struct meta_repeator
			{
				using type = typename meta_looper_t<meta_length_limiter_o<N>,
					meta_appendable_o<repeat_list<>>, generate_self_o<T>>::type;
			};
			template<size_t N, class ...T> struct meta_repeator<N, repeat_mode<T...>>
			{
				struct append_f
				{
					template<class thisMO, class TL>
					using apply = typename meta_all_transfer<op_stream<thisMO>, ip_stream<TL>>::to::type;
				};
				using type = typename meta_timer_looper_t<
					meta_timer_object<N, repeat_list<>, append_f>,
					generate_self_o<repeat_mode<T...>>
				>::type;
			};

			

			template<class T>
			struct is_repeat_list : std::false_type{};

			template<class ...T>
			struct is_repeat_list<repeat_list<T...>> :std::true_type {};

			template<class T>
			constexpr bool is_repeat_list_v = is_repeat_list<T>::value;
			
			struct append_repeator
			{
				template<class thisMO, class T> struct accept_t 
				{
					using type = append::template apply<thisMO, T>;
				};
				template<class thisMO, class repl> struct accept_repl
				{
					static constexpr size_t times = size_of_type_list<repl>::value;
					using timer_add_o = meta_timer_object<times, thisMO, append>;
					using type = typename meta_timer_looper_t<
						timer_add_o,
						meta_ret_decreasible_o<typename exp_rename<repl, decrease_list>::type>
					>::type;
				};
				template<class thisMO, class T>
				using apply = typename meta_if<is_repeat_list_v<T>>::template apply<
					accept_repl<thisMO, T>,
					accept_t<thisMO, T>
				>::type;
			};

			template<class TL>
			using meta_repeatible_append_o = meta_object<TL, append_repeator>;
			template<size_t times, class TL>
			using meta_repeatible_append_timer_o = meta_timer_object<times, TL, append_repeator>;

			template<size_t N, class ...> struct repeat_impl {};
			template<size_t N, class T> struct repeat_impl<N,T> 
			{
				using type = typename meta_repeator<N, T>::type;
			};
			template<size_t N, class T1, class T2, class ...Tx> 
			struct repeat_impl<N, T1, T2, Tx...>
			{
				using type = typename meta_repeator<N, repeat_mode<T1, T2, Tx...>>::type;
			};

			template<size_t N, class...T>
			using do_repeat = typename repeat_impl<N, T...>::type;
			
			template<class TL> struct decl_repeat {};
			
			template<template<class...> class TL, class ...T> struct decl_repeat<TL<T...>>
			{
				using ip = meta_ret_decreasible_o<TL<T...>>;
				using op = meta_repeatible_append_o<TL<>>;
				using type = typename meta_all_transfer<op, ip>::to::type;
			};
			template<class ...Typs> using repeat_raw = typename decl_repeat<exp_list<Typs...>>::type;
			
		}
		namespace delimiter
		{
			template<class type_delimiter> struct delimiter_append_f
			{
				template<class thisMO, class T, class...>
				using apply = typename meta_if<(size_of_type_list<thisMO>::value != 0)>
					::template apply<
					exp_join<thisMO, exp_list<type_delimiter, T>>,
					exp_join<thisMO, exp_list<T>>
					>::type;
			};
			template<class TL, class type_delimiter> using delimiter_op_stream = meta_object<
				get_type<exp_empty<TL>>, 
				delimiter_append_f<type_delimiter>
			>;
			template<class TL> using delimiter_ip_stream = ip_stream<TL>;

			template<class TL, class type_delimiter> struct delim_impl 
			{
				using type = typename meta_all_transfer<delimiter_op_stream<TL, type_delimiter>, delimiter_ip_stream<TL>>::to::type;
			};

			template<class ...Typs> struct delim_list
			{
				template<class type_delimiter> using apply = get_type<
					delim_impl<exp_list<Typs...>, type_delimiter>
				>;
			};
		

		}
		namespace join
		{
			
			struct join_append_f
			{
				template<class ThisMO, class T> struct join_append
				{
					using type = get_type<
						exp_join_impl<ThisMO, T>
					>;
				};
				template<class ThisMO, template<class ...> class TL, class ...L> 
				struct join_append<ThisMO, TL<L...>>
				{
					using type = get_type<
						exp_join<ThisMO, TL<L...>>
						>;
				};
				template<class ThisMO, class T, class...> using apply = get_type<join_append<ThisMO, T>>;
			};
			template<class TL> using meta_join_append_o = meta_object<typename exp_empty<TL>::type, join_append_f>;

			template<class ...Typs> struct join_list_impl :exp_list<Typs...> 
			{
				using op = meta_join_append_o<exp_list<Typs...>>;
				using ip = ip_stream<exp_list<Typs...>>;
				using type = typename meta_all_transfer<op, ip>::to::type;
			};

			template<class ...Typs> using join_list = get_type<join_list_impl<Typs...>>;

			template<class... typs> struct all_is_true_list :exp_list<typs...>
			{
				template<template<class> class F> struct apply
				{
					static constexpr bool value = std::conjunction_v<F<typs>...>;
				};
			};

			template<class T> struct is_not_typelist : std::true_type {};
			template<template<class...> class TL, class ...TS> 
			struct is_not_typelist<TL<TS...>> : std::false_type {};

			template<class TL> struct is_exp_list_based: std::false_type{};
			template<template<class ...> class TL, class ...TS> 
			struct is_exp_list_based<TL<TS...>> 
			{
				static constexpr bool value = std::is_base_of_v<exp_list<TS...>, TL<TS...>>;
			};
			template<class TL> constexpr bool is_exp_list_based_v = is_exp_list_based<TL>::value;

			template<class exp_list_based_tl> requires is_exp_list_based_v<exp_list_based_tl>
			constexpr bool is_all_joined_v = exp_list_based_tl
				::template to<all_is_true_list>
				::template apply<is_not_typelist>
				::value;

			//recursivly separate all typelist elements in a typelist
			//requires a typelist based on exp_list
			template<class TL> requires is_exp_list_based_v<TL> 
			struct final_join
			{
				struct is_not_final_joined_cof {
					template<class ThisObj, class...>
					struct apply
					{
						static constexpr bool value = !is_all_joined_v<ThisObj>;
					};
				};
		
				struct final_joined_transform_of
				{
					template<class ThisObj, class...>
					using apply = ThisObj::template to<join::join_list>;
				};

				using is_not_final_joined_co = meta_object<TL, is_not_final_joined_cof>;
				using final_joined_transform_o = meta_object<TL, final_joined_transform_of>;

				using type = typename meta_while<is_not_final_joined_co, final_joined_transform_o>::type;
			};

		}
		
		namespace static_wrap
		{
			template<char WL, char WR> struct chars_wrapper
			{
				static_assert((WL != '{' && WR != '}'),"forbid using of format default contol characters");
				using left = char_val<WL>;
				using right = char_val<WR>;
				template<class T> using apply = chars<left, T, right>;
			};

			using brackets_wrapper = chars_wrapper<'(', ')'>;
			using square_brackets_wrapper = chars_wrapper<'[', ']'>;

			template<class ...cs> struct wrap_list : exp_list<cs...>
			{
				template<class F> using apply = exp_fn_apply<F, exp_list<cs...>>;
				template<class idx_type_container, class wrap_f> struct with_indices
				{};

				template<template<class...> class idx_type_container, class wrap_f, class ...TS>
				struct with_indices<idx_type_container<TS...>, wrap_f>
				{
					struct wrap_with_indices_f
					{
						template<class TO, class idx_type, class...>
						struct apply_impl { using type = TO; };

						template<class TO, template<size_t> class idx_type, size_t I, class ...TS>
						struct apply_impl<TO, idx_type<I>, TS...>
						{
							using type = TO
								::template invoke<I>
								::template transform_to<wrap_f>;
						};
						
						template<class TO, class idx_type, class...TS>
						using apply = typename apply_impl<TO, idx_type, TS...>::type;
					};
					template<class TL> using wrap_op_stream = meta_object<
						to_selectable_t<TL>, 
						wrap_with_indices_f
					>;
					using idx_ip_stream = ip_stream<idx_type_container<TS...>>;

					using wrap = meta_all_transfer<wrap_op_stream<exp_list<cs...>>, idx_ip_stream>::template to::type;

				};
			};
		}
		template<size_t N>
		using meta_create_sequence = exp_repeat::meta_to_array<exp_repeat::meta_itoa<N>>;
		namespace tag
		{
			template<class TL, class idx_container> struct tag_list_impl {};

			template<template<class...> class TL, template<size_t ...> class idx_container, class ...TS, size_t ...I>
			struct tag_list_impl<TL<TS...>, idx_container<I...>>
			{
				using type = exp_list<meta_tag<I, TS>...>;
			};

			template<class ...TS> struct tag_list_type: exp_list<TS...>
			{
				using type = typename tag_list_impl<exp_list<TS...>, meta_create_sequence<sizeof...(TS) - 1>>::type;
			};

			template<class ...TS> using tag_list = typename tag_list_type<TS...>::type;
		}
		namespace select_if_space
		{
			template<template<class> class F> struct select_if_f
			{
				template<class TO, class T, class ...> struct apply : std::false_type{};
				template<class TO, template<size_t, class> class idx_tag, size_t I, class T, class ...TS>
				struct apply<TO, idx_tag<I, T>, TS...>
				{
					//a fliter flites all elements that follow the meta functions rule
					//so a negation is needed to get types those follow the functions rule
					static constexpr bool value = !F<T>::value;
				};
			};

			template<template<class> class F> using select_if_op = meta_appendable_fliter_o<exp_list<>, select_if_f<F>>;
			template<class TL> using select_if_ip = meta_ret_decreasible_o<TL>;

			//to use this feature, a typelist must be tagged by indices
			//to prevent repeat types in a typelist
			template<template<class> class F,class TL>
			using select_if_impl =typename meta_all_transfer<select_if_op<F>, select_if_ip<TL>>::template to::type;

			//only indices are what needed so types will be erased
			template<class> struct get_rid_of_type {};
			template<template<size_t, class> class tag_list_t, size_t ...I, class ...TS>
			struct get_rid_of_type<exp_list<tag_list_t<I, TS>...>>
			{
				using type = meta_array<I...>;
			};

			template<template<class> class F, class TL> using select_if_list = typename get_rid_of_type<
				select_if_impl<F, TL>
			>::type;
		}
		namespace partial
		{
			template<size_t N, size_t L> struct partially_split
			{
				template<class TL> requires ((N + L) <= exp_size<TL>)
				struct split
				{
					using front = typename meta_spliter<N>::template front<TL>;
					using back = typename meta_spliter<N + L>::template back<TL>;
					using join = typename meta_all_transfer<op_stream<front>, ip_stream<back>>::to::type;
				};
			};
			template<class ...Typs> struct partially : exp_list<Typs...>
			{
				template<size_t N, size_t L> struct at 
				{
					template<class F> struct transform_impl
					{
						using type = join::join_list<
							typename partially_split<N, L>::template split<exp_list<Typs...>>::front,
							meta_invoke<F, typename list_slice<N, L>::template apply<exp_list<Typs...>>>, 
							typename partially_split<N, L>::template split<exp_list<Typs...>>::back
						>;
					};
					template<class F> using transform = typename transform_impl<F>::type;
				};
			};
		}
		namespace grouper
		{
			template<class TL> struct group
			{
				template<size_t N> struct group_decrease
				{
					template<class ThisTL, bool rest_is_enough_to_decrease> struct group_decrease_impl
					{
						using type = typename meta_stream_transfer_mo<
							meta_stream_o<N, op_stream<exp_list<>>,
							ip_stream<ThisTL>>
							>::from::type;
					};
					template<class ThisTL> struct group_decrease_impl<ThisTL, false>
					{
						using type = ThisTL;
					};
					template<class ThisTL, class...> using apply = typename group_decrease_impl<ThisTL, (exp_size<ThisTL> >= N)>::type;
				};

				template<size_t N> struct group_decrease_ret
				{
					template<class ThisTL, bool rest_is_enough_to_ret> struct group_decrease_ret_impl
					{
						using type = meta_stream_transfer<
							meta_stream_o<N, op_stream<exp_list<>>,
							ip_stream<ThisTL>>
							>;
					};

					template<class ThisTL> struct group_decrease_ret_impl<ThisTL, false>
					{
						using type = ThisTL;
					};
					template<class ThisTL, class...> using apply = typename group_decrease_ret_impl<ThisTL, (exp_size<ThisTL> >= N)>::type;
				};
				template<size_t N>
				using group_stream_o = meta_stream_o<
					(exp_size<TL> / N),
					op_stream<exp_list<>>,
					meta_ret_object<TL, group_decrease<N>,
					group_decrease_ret<N>>
					>;

				template<size_t N> using devide_impl = meta_stream_transfer<group_stream_o<N>>;


				template<size_t N> struct group_type {
					using grouped = devide_impl<N>;
					using rest = typename meta_stream_transfer_mo<group_stream_o<N>>::from::type;
				};
			};
		}
		template<const char* sptr, size_t ...indices>
		struct static_str_impl
		{
			using type = chars <char_val<sptr[indices]>...>;
		};

		template<class idx_container, const char* str> struct static_str_bind {};

		template<template<size_t...> class idx_container, const char* str, size_t ...indices>

		struct static_str_bind<idx_container<indices...>, str> {
			using type = typename static_str_impl<str, indices...>::type;
		};

		template<size_t N, const char* str>
		using static_str = typename static_str_bind<meta_create_sequence<N - 1>, str>::type;

		template<class char_list, size_t ...I> struct static_to_str_impl
		{
			using iter = typename char_list::chars_to_selectable;
			inline static const char str[]{ iter::get<I>::value... };
		};

		template<class char_list, class idx_container> struct static_to_str_bind {};
		template<class char_list, template<size_t...> class idx_container, size_t... I>
		struct static_to_str_bind<char_list, idx_container<I...>>
		{
			using type = static_to_str_impl<char_list, I...>;
		};

		template<class char_list, size_t N>
		using static_to_str = static_to_str_bind<char_list, meta_create_sequence<N - 1>>;

		template<class CL1, class CL2>
		constexpr auto static_combine(CL1, CL2) -> get_type<exp_join<CL1, CL2>>
		{
			return {};
		}

		template<size_t N, class CL1, class CL2> requires ((size_of_type_list<CL1>::value) >= N)
		struct insert_impl
		{
			using CL1_front = meta_spliter<N>::front<CL1>;
			using CL1_back = meta_spliter<N>::back<CL1>;
			using add_CL2_to_CL1_front = typename meta_all_transfer<op_stream<CL1_front>, ip_stream<CL2>>::to::type;
			using type = typename meta_all_transfer<op_stream<add_CL2_to_CL1_front>, ip_stream<CL1_back>>::to::type;
		};
		template<size_t N, class CL1, class CL2>
		constexpr auto static_insert(CL1, CL2) -> get_type<insert_impl<N, CL1, CL2>>
		{
			return {};
		}


		
	}

	template<size_t ...I, class ...Args>
	constexpr std::string format_output_impl(std::integer_sequence<size_t, I...>, Args ... args)
	{
		using brackets = typename generate_brackets<sizeof...(args)>::chars_to_selectable;
		static const char format_str[]{brackets::get<I>::value..., '\0'};
		return std::format(format_str, args...);
	}
	template<class ...Args>
	constexpr std::string format_output(Args...args)
	{
		return format_output_impl(std::make_index_sequence<sizeof...(args)*2>{}, args...);
	}

	

	//the fstring is a string constains various types of data
	//each element in the string accessible through providing index 
	template<class ...Typs> requires check_formatible_v<Typs...>
	struct fstring :VH_NODE::node_struct<Typs...>
	{
		fstring() {}
		fstring(Typs&&...typs) :VH_NODE::node_struct<Typs...>(std::move(typs)...) {}

		using fs_bracket = meta_string_stream::repeator::repeat_list<lbracket, rbracket>;



		//use a static const string as control
		template<const char* fsfmt> std::string fmt_string()
		{
			auto string_maker = []<class ...Typs>(Typs&&...typs)
			{
				return std::format(fsfmt, std::move(typs)...);
			};
			auto binder = exp_bind::bind(
				function_impl::realize_meta<
				exp_list<Typs...>
				>(string_maker)
			);
			loop_with(VH_NODE::node_struct<Typs...>::members.next_element(),
				[&binder](auto& value) {binder.bind(value); });
			return binder();
		}

		//create a basic control string to show all elements in fstring
		//can be modified 
		constexpr auto control_str() const-> chars<
			meta_string_stream::repeator::do_repeat<sizeof...(Typs), fs_bracket>
		>::template to<meta_string_stream::repeator::repeat_raw>
		{
			return{};
		}

		template<class s_str, template<size_t...> class idx_v, size_t ...I>
		std::string static_string(idx_v<I...>) const
		{
			auto string_maker = []<class ...Typs>(Typs&&...typs)
			{
				using iter = typename s_str::template to<selectable_list>;
				static const char str[]{ iter::get<I>::value...,0 };
				return std::format(str, std::move(typs)...);
			};
			auto binder = exp_bind::bind(
				function_impl::realize_meta<
				exp_list<Typs...>
				>(string_maker)
			);
			loop_with(VH_NODE::node_struct<Typs...>::members.next_element(),
				[&binder](auto& value) {binder.bind(value); });
			return binder();
		}

		//use exp_chars as control
		template<class s_str>
		std::string exp_to_string() const
		{
			return this->static_string<s_str>(meta_string_stream::meta_create_sequence<s_str::length - 1>{});
		}
		//out put every element as string
		std::string to_string() const
		{
			auto string_maker = []<class...Typs>(Typs&&...typs) {
				return format_output(std::move(typs)...);
			};
			auto binder = exp_bind::bind(
				function_impl::realize_meta<
				exp_list<Typs...>
				>(string_maker)
			);
			loop_with(VH_NODE::node_struct<Typs...>::members.next_element(),
				[&binder](auto& value) {binder.bind(value); });
			return binder();
		}

		template<template<class> class transform_fn > auto transformed_string() const
		{
			using original_cs = decltype(control_str());
			return std::move(exp_to_string<transform_fn<original_cs>>());
		}
		template<template<class, class> class transformed_this_fn> auto transformed_string() const 
		{
			return std::move(exp_to_string<transformed_this_fn<decltype(control_str()), fstring<Typs...>>>());
		}
		
	};
	template<class ...Typs>
	fstring(Typs&& ...) -> fstring<Typs...>;

	template<template<class...> class F> struct ctr_wrapper {};

	template<template<class...> class FMTCTR, class ...Typs> struct format_fstring : fstring<Typs...>
	{
		format_fstring() {}
		format_fstring(ctr_wrapper<FMTCTR>, Typs&&... args) :fstring<Typs...>(std::move(args)...) {}

		std::string to_string() const
		{
			return this->transformed_string<FMTCTR>();
		}
	};

	template<template<class...> class FMTCTR, class ...Typs> format_fstring(ctr_wrapper<FMTCTR>, Typs&&... args) -> format_fstring<FMTCTR, Typs...>;


	template<class arr_like> struct array_to_fstring {};

	template<template<class, size_t> class array_like, class T, size_t N>
	struct array_to_fstring<array_like<T, N>>
	{
		using type = meta_string_stream::repeator::repeat_raw<
			meta_string_stream::repeator::do_repeat<N, T>
		>::template to<fstring>;
	};

	
	struct same_type_fstring
	{
		template<class ...Tys>
		auto operator()(Tys...args)->fstring<Tys...>
		{
			return { std::move(args)... };
		}
	};


	template<template<class, size_t> class array_like, class T, size_t N>
	auto array_fstring(array_like<T, N> const& arr)
	{
		auto fs_creator = exp_bind::bind(
			function_impl::realize_meta<
			meta_string_stream::repeator::repeat_raw<
			meta_string_stream::repeator::do_repeat<N, T>
			>
			>(same_type_fstring{}));
		for (const T& i : arr)
		{
			fs_creator.bind(i);
		}
		return fs_creator.apply_func();
	}

	template<size_t ...I, class ...Typs>
	auto tuple_fstring_impl(std::tuple<Typs...> tp, meta_array<I...> ma)
	{
		return fstring{ std::move(std::get<I>(tp))... };
	}

	template<class TP> auto tuple_fstring(TP const& tp)
	{
		return tuple_fstring_impl(tp, meta_create_sequence<exp_size<TP> - 1>());
	}


	//get a string with array delim with a char type
	//char type must provide ::value of char
	template<template<class, size_t> class array_like, class T, size_t N, class char_delimiter>
	std::string array_delim(array_like<T, N> const& arr, char_delimiter cd)
	{
		auto fs_array = array_fstring(arr);

		using delim_control = decltype(fs_array.control_str())
			::template to<meta_string_stream::delimiter::delim_list>::template apply<char_delimiter>
			::template to<meta_string_stream::join::join_list>
			::template to<chars>;
		return fs_array.exp_to_string<delim_control>();
	}
	template<template<typename> typename sstr_f, typename TP> std::string tuple_format(TP const& tp)
	{
		return tuple_fstring(tp).transformed_string<sstr_f>();
	}
	template<template<typename, typename> typename sstr_f, typename TP> std::string tuple_format(TP const& tp)
	{
		return tuple_fstring(tp).transformed_string<sstr_f>();
	}


	
	

	
#define	EXP_STATIC_STR(x) static_str<(sizeof x) - 1, x> 
#define	EXP_STATIC_TO_STR(x) static_to_str<x, x::length>::type::str


	template<class TL> using fs_final = typename meta_string_stream::join::final_join<TL>::type
		::template to<chars>;

	namespace common_utility
	{
		struct ip_format_type
		{
			static constexpr char str[12]{ "{}.{}.{}.{}" };
			using type = meta_string_stream::EXP_STATIC_STR(str);
			using IPV4 = fstring<short, short, short, short>;
		};
		
		using ipv4_format = ip_format_type::type;
		using ipv4_t = ip_format_type::IPV4;
	}
	namespace flex_string_space
	{
		using namespace meta_string_stream;
		using namespace repeator;
		using namespace delimiter;
		using namespace join;
		using namespace tag;
		using namespace select_if_space;
		using namespace static_wrap;
		using namespace partial;
		using namespace grouper;
	}
}
template<template<typename ...> typename F, typename ...Typs>
struct std::formatter<flex_string::format_fstring<F, Typs...>>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(flex_string::format_fstring<F, Typs...> const& ffstr, FormatContext& fctx) const
	{
		return format_to(fctx.out(), "{}", ffstr.to_string());
	}
};