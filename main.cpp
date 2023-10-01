
#include<iostream>
#include"meta_object_traits.hpp"
#include"exp_print.hpp"

using namespace meta_traits;
using namespace meta_traits::common_object;
#define complie_var static const

template<size_t TL_Length>
struct tag_type
{
	template<class TL> struct apply_impl
	{
		complie_var size_t idx = TL_Length - size_of_type_list<TL>::value;
		using type = exp_list<exp_repeat::Idx<idx>, decrease_ret::apply<TL>>;
	};
	template<class TL,class...>
	using apply = typename apply_impl<TL>::type;
};



int main()
{
	using l = decrease_list<int, double, char>;//
	using m = exp_list<int, double, char>;
	using ty2 = typename meta_looper_t<meta_length_limiter_o<5>, //从l中转移类型给m，直到m的长度==5
		meta_appendable_o<m>, 
		meta_ret_decreasible_o<l>
	>::type;
	ty2 y2{};
	exp_print::print_type(y2);
	using timer_append_o = meta_timer_object<2, m, append>; //从l中转移类型给m，2次
	using ty3 = meta_looper_t<meta_timer_cnd_o, 
		timer_append_o, 
		meta_ret_decreasible_o<l>
	>::type;
	ty3 y3{};
	exp_print::print_type(y3);
	using tag_type_o = meta_ret_object<l, decreased, tag_type<2>>;
	using timer_append_o2 = meta_timer_object<3, exp_list<>, append>;
	using ty4 = typename meta_looper_t<meta_timer_cnd_o, timer_append_o2, tag_type_o>::type;
	ty4 y4{};

	using v = exp_select<0, exp_select<0, ty4>>;

	exp_print::print_type(y4);

}