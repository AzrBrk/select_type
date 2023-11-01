#pragma once

#include"exp_vh_node.hpp"

namespace flex_string
{
	template<class T>
	concept formatible = requires(std::ostream & os, T && ty) { os << ty; };

	template<class T> struct is_formatible { static const bool value = formatible<T>; };
	template<class ...Typs> constexpr bool check_formatible_v = std::conjunction_v<is_formatible<Typs>...>;

	using namespace meta_typelist;
	template<char c> struct char_val { static constexpr char value = c; };

	template<class ...char_vals> struct chars : exp_list<char_vals...> {
		using chars_to_selectable = selectable_list<char_vals...>;
	};

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
			template<class...> struct repeat_list {};
			template<class...> struct repeat_mode {};
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
		template<size_t N>
		using meta_create_sequence = exp_repeat::meta_to_array<exp_repeat::meta_itoa<N>>;
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
	struct fstring:VH_NODE::node_struct<Typs...>
	{
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
		
		//create a basic control string to show all element in fstring
		//can be modified 
		constexpr auto control_str()->chars<
			meta_string_stream::repeator::do_repeat<sizeof...(Typs), fs_bracket>
		>::template to<meta_string_stream::repeator::repeat_raw>
		{
			return{};
		}
		
		template<class s_str, template<size_t...> class idx_v, size_t ...I>
		std::string static_string(idx_v<I...>)
		{
			auto string_maker = []<class ...Typs>(Typs&&...typs)
			{
				using iter = typename s_str::chars_to_selectable;
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
		std::string exp_to_string()
		{
			return this->static_string<s_str>(meta_string_stream::meta_create_sequence<s_str::length - 1>{});
		}
		//out put every element as string
		std::string to_string()
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
		
	};

	template<class ...Typs>
	fstring(Typs&& ...) -> fstring<Typs...>;
#define	EXP_STATIC_STR(x) static_str<(sizeof x) - 1, x> 
#define	EXP_STATIC_TO_STR(x) static_to_str<x, x::length>::type::str;

	template<class TL> using fs_final = TL::template to<meta_string_stream::repeator::repeat_raw>::template to<chars>;

	namespace common_utility
	{
		struct ip_format_type
		{
			static constexpr char str[12]{ "{}.{}.{}.{}" };
			using type = meta_string_stream::EXP_STATIC_STR(str);
			using IPV4 = fstring<short, short, short, short>;
		};
		
		using ip_format = ip_format_type::type;
		using ipv4_t = ip_format_type::IPV4;
	}
}