#pragma once
#include"select_type.hpp"

namespace exp_bind{
	template<class F>
	struct exp_function_binder {};
	
	template<class R, class ...L>
	struct exp_function_binder<R(L...)>
	{
		static_assert(sizeof ...(L) != 0, "exp_function_binder does not support empty arguments,\
		it's aiming at manipulating function arguments,\
		use std::function instead");
		using type = std::function<R(L...)>;
		std::tuple<L...> args_stack{};
		tuple_iterator<std::tuple<L...>> args_iter_keep{ args_stack };
		type func_binder;
		using func_type = R(L...);

		size_t current_index{ 0 };

		void rebind_back(size_t back_offset)
		{
			if (back_offset > current_index) throw std::exception("argument stack overflow!");
			current_index -= back_offset;
		}

		exp_function_binder(func_type _f) :func_binder(_f) {}

		exp_function_binder(type stdfunc) :func_binder(stdfunc) {};

		typename tuple_iterator<std::tuple<L...>>::iterator_type& operator[](size_t index)
		{
			return args_iter_keep.iterator()[index];//return iterator of function stack
		}

		template<class T>
		void bind(T val)
		{
			auto& iter = args_iter_keep.iterator();
			if (current_index >= iter.size()) return;
			iter[current_index++] = std::move(val);
		}
		
		template<class...L>
		void bind_a_lot(L...l)
		{
			(bind(l), ...);
		}

		R apply_func()
		{
			return std::apply([*this](auto&& ...l) { return func_binder(l...); }, std::move(args_stack));
		}

		template<class ...L>
		R operator()(L...l)
		{
			bind_a_lot(l...);
			return apply_func();
		}
	};

	template<class T, class F>
	struct member_function_wrapper
	{};
	template<class T, class R, class ...L>
	struct member_function_wrapper<T, R(T::*)(L...)>
	{
		using func_type = R(T::*)(L...);
		T& obj_ref;
		func_type call;
		member_function_wrapper(T& obj, func_type mf) :obj_ref(obj), call(mf) {}
		template<class ...AL>
		R apply(AL...al)
		{
			return (obj_ref.*call)(al...);
		}
	};

	template<class T, class F>
	struct exp_member_function_wrapper
	{};


	template<class T, class R, class ...L>
	struct exp_member_function_wrapper<T, R(T::*)(L...)>
	{

		member_function_wrapper<T, R(T::*)(L...)> wrapper;
		using func_type = R(T::*)(L...);
		exp_member_function_wrapper(T& obj, func_type mf) :wrapper(obj, mf) { apply(); }

		std::function<R(L...)> func;

		void apply() {
			auto& _wrapper = wrapper;
			auto lamb = [&_wrapper](L...l) { return _wrapper.apply(l...); };
			func = lamb;
		}
	};


	template<class T, class R, class ...L>
	using decl_mem_type = R(T::*)(L...);
	template<class R, class ...L>
	using decl_fn_type = R(L...);

	template<class _std_fn>
	struct _std_fn_unwrapper
	{};
	
	template<class fn>
	struct _std_fn_unwrapper<std::function<fn>>
	{
		using fn_type = fn;
		std::function<fn> fnc;
		_std_fn_unwrapper(std::function<fn> f):fnc(f){}
	};
	
	template<class R, class ...L>
	exp_function_binder<decl_fn_type<R, L...>> bind(decl_fn_type<R, L...> f)
	{
		return { f };
	}
	template<class F>
	auto bind(F&& f)
	{
		std::function ff = f;
		_std_fn_unwrapper<decltype(ff)> sf(ff);
		return exp_function_binder<typename decltype(sf)::fn_type>{ff};
	}
	template<class T, class R, class ...L>
	exp_member_function_wrapper<T, decl_mem_type<T, R, L...>> exp_mem_wrap(T& obj, decl_mem_type<T, R, L...> mf)
	{
		return { obj, mf };
	}
	template<class Wrapper>
	auto bind_mem(Wrapper& wp)
	{
		_std_fn_unwrapper<decltype(wp.func)> _sfu(wp.func);
		exp_function_binder<decltype(_sfu)::fn_type> efb(wp.func);
		return efb;
	}
}
