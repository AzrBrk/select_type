#include <iostream>
#include "exp_function_series.hpp"
#include "select_type.hpp"
#include "exp_print.hpp"
#include "function_impl.hpp"
#include"exp_function_binder.hpp"
#include<map>
#include <utility>
#include <type_traits>
#include <string>
#include<stacktrace>
#include<ranges>
#include<iostream>

template<class TL, class T>
struct type_push {};

template<template<class ...> class TL, class T, class ...L>
struct type_push<TL<L...>, T>
{
	using type = TL<L..., T>;
};
template<class TL, class T>
using push_back_ = typename type_push<TL, T>::type;

template<class T, class TL, size_t N>//generate typlist<T,T,T,T.....>
struct type_c
{
	using type = typename type_c<T, push_back_<TL, T>, N - 1>::type;
};
template<class T, class TL>
struct type_c<T, TL, 0>
{
	using type = TL;
};

int main()
{
	using args =typename type_c<int, exp_list<>,200>::type; //创建的一个含有150个int的类型列表
	auto add_a_lot = [](auto && ...x) {return (x + ... + 0); };//随便lambda
	
	auto efb = exp_bind::bind(function_impl::realize_meta<args>(add_a_lot));//实例化lambda模板的参数为以上列表
	std::cout << typeid(efb).name();
	for (auto i : std::views::iota(0, 200))
	{
		efb.bind(i);//尝试绑参数
	}
	std::cout<<'\n'<<efb();//尝试运行
}