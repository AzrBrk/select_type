#pragma once
#include"exp_function_binder.hpp"
#include"exp_function_series.hpp"
#include"exp_repeat.hpp"
#include"select_type.hpp"
#include <concepts>
#include<sstream>

namespace exp_operator
{
	using namespace exp_bind;
	using namespace exp_function_series;

	template<class R, class T, class ...Args> requires std::invocable<R(T, Args...), T, Args...>
	auto& bind_except_first(exp_function_binder<R(T, Args...)>& exp_binder, Args ... args)
	{
		exp_binder.reset_argc_stack();
		++exp_binder.current_index;//skip node
		exp_binder.bind_a_lot(std::forward<Args>(args)...);
		exp_binder.reset_argc_stack();
		return exp_binder;
	}

	template<class node, class exp_fn>
	void deal_iter(exp_iterator<node>& iter, exp_fn& f)
	{
		auto bind_exp_binder = [&f](auto& value)
			{
				f.bind(value);
			};
		do_at(iter.first_node, f, iter.exp_index);
	}
	template<class node, class T>
	T operator+ (exp_iterator<node>& iter, T const& val)
	{
		auto efb = exp_bind::bind([](T x, T y) {return x + y; });
		deal_iter(iter, bind_except_first(efb, val));
		return efb.apply_func();
	}
	template<class node, class T>
	exp_iterator<node>& operator +=(exp_iterator<node>& iter, T const& val)
	{
		iter = iter + val;
		return iter;
	}
	template<class node, class T>
	T operator- (exp_iterator<node>& iter, T const& val)
	{
		auto efb = exp_bind::bind([](T x, T y) {return x - y; });
		deal_iter(iter, bind_except_first(efb, val));
		return efb.apply_func();
	}
	template<class node, class T>
	exp_iterator<node>& operator -=(exp_iterator<node>& iter, T const& val)
	{
		iter = iter - val;
		return iter;
	}

	template<class node, class T>
	T operator* (exp_iterator<node>& iter, T const& val)
	{
		auto efb = exp_bind::bind([](T x, T y) {return x * y; });
		deal_iter(iter, bind_except_first(efb, val));
		return efb.apply_func();
	}
	template<class node, class T>
	exp_iterator<node>& operator *=(exp_iterator<node>& iter, T const& val)
	{
		iter = iter * val;
		return iter;
	}
	template<class node, class T>
	T operator> (exp_iterator<node>& iter, T const& val)
	{
		auto efb = exp_bind::bind([](T x, T y)->bool {return x > y; });
		deal_iter(iter, bind_except_first(efb, val));
		return efb.apply_func();
	}
	template<class node, class T>
	T operator< (exp_iterator<node>& iter, T const& val)
	{
		return !(iter > val);
	}

	template<class R, class STR>
		requires std::is_same_v<std::decay_t<STR>, std::string>
	std::istringstream& operator>>(std::istringstream& iss, exp_function_binder<R(STR)>& efb)
	{
		efb.bind(iss.str());
		iss.clear();
		return iss;
	}
	template<class R>
	std::istream& operator>>(std::istream& is, exp_function_binder<R()>& efb)//input to a empty stack function is ...
	{
		return is;
	}


	template<class ...EFBS>
	std::istream& operator>>(std::istream& is, series<EFBS...>& fl)
	{
		auto cinefb = [&is](auto& efb)
			{
				for (size_t i = 0; i < efb.argc_count(); ++i)
				{
					is >> efb;
				}
			};
		do_at(*fl, cinefb, fl.series_index);
		return is;
	}
	template<class ...EFBS>
	std::istringstream& operator>>(std::istringstream& iss, series<EFBS...>& fl)
	{
		auto cinefb = [&iss](auto& efb)
			{
				for (size_t i = 0; i < efb.argc_count(); ++i)
				{
					iss >> efb;
				}
			};
		do_at(*fl, cinefb, fl.series_index);
		return iss;
	}
	

	template<class T, class U>
	bool operator == (ref_wrapper<T> rw, U u)
	{
		if constexpr (requires(T x, U y)
		{
			{x == y}->std::same_as<bool>;
		}
		){
			return rw.value == u;
		}
		return false;
	}


	
}