#pragma once
#include"select_type.hpp"

namespace exp_bind{
	template<class F>
	struct exp_function_binder {};

	template<class R>
	struct exp_function_binder<R()>
	{
		using func_type = R();
		using type = std::function<R()>;
		using return_type = R;
		using argument_list_type = std::tuple<void*>;
		std::tuple<void*> args_stack{};
		type func_binder;
		exp_function_binder(func_type _f) :func_binder(_f) {}
		exp_function_binder(type stdfunc) :func_binder(stdfunc) {};
		size_t argc_count() { return 0; }

		R apply_func() { return func_binder(); }
		R operator()() { return apply_func(); }
	};



	template<class R, class ...L>
	struct exp_function_binder<R(L...)>
	{
		using type = std::function<R(L...)>;
		using argument_list_type = exp_list<L...>;
		using func_type = R(L...);
		using return_type = R;
		exp_function_binder(func_type _f) :func_binder(_f) {}
		exp_function_binder(type stdfunc) :func_binder(stdfunc) {}

		//Since EFB uses reference to arg_stack
		//A default copy operation causes error
		exp_function_binder(exp_function_binder&& efb) :
			args_stack(efb.args_stack),
			current_index(efb.current_index),
			func_binder(efb.func_binder),
			args_iter_keep(args_stack)
		{}
		exp_function_binder(const exp_function_binder& efb) :
			args_stack(efb.args_stack),
			current_index(efb.current_index),
			func_binder(efb.func_binder),
			args_iter_keep(args_stack)
		{}

		std::tuple <auto_ref_unwrapper<L> ... > args_stack{};
		tuple_iterator<std::tuple<auto_ref_unwrapper<L>...>> args_iter_keep{ args_stack };
		type func_binder;
		


		size_t current_index{ 0 };

		size_t argc_count() { return args_iter_keep.iterator().size(); }

		void reset_argc_stack() { current_index = 0; }

		void rebind_back(size_t back_offset)
		{
			if (back_offset > current_index) throw std::exception("argument stack overflow!");
			current_index -= back_offset;
		}

	

		typename tuple_iterator<std::tuple<L...>>::iterator_type& operator[](size_t index)
		{
			return args_iter_keep.iterator()[index];//return iterator of function stack
		}

		friend std::istream& operator>>(std::istream& ins, struct exp_function_binder<R(L...)>& efb)
		{
			ins >> efb[efb.current_index++];
			return ins;
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
			(bind(std::forward<L>(l)), ...);
		}

		R apply_func()
		{
			current_index = 0;
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
		template<class ...AL>
		R operator()(AL ...al)
		{
			return apply(al...);
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
		R operator()(L... l)
		{
			return func(std::forward<L>(l)...);
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
	//lambda support, use std::function to convert lambda
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

	template<class T, class R, class ...L>
	auto bind_mfw(T& obj, decl_mem_type<T, R, L...> mf)
	{
		return [&obj, mf](L...l)->R {return member_function_wrapper{ obj, mf }(l...); };
	}

	template<class T, class F>
	auto bind(T& obj, F f)
	{
		return bind(bind_mfw(obj, f));
	}
	template<class T, class R, class ...L>
	member_function_wrapper(T& obj, decl_mem_type<T, R, L...> mf) -> member_function_wrapper<T, R(T::*)(L...)>;

	template<class T, class R, class ...MFA>
	exp_member_function_wrapper(T& obj, decl_mem_type<T, R, MFA...> mf) -> exp_member_function_wrapper<T, decl_mem_type<T, R, MFA...>>;
	template<class R, class ...L>
	exp_function_binder(decl_fn_type<R, L...> fn) -> exp_function_binder<decl_fn_type<R, L...>>;
	template<class F>
	exp_function_binder(std::function<F> f) -> exp_function_binder<typename _std_fn_unwrapper<typename decltype(f)>::fn_type>;
}
