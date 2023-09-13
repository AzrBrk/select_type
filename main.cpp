#include <iostream>
#include "exp_function_series.hpp"
#include "select_type.hpp"
#include "exp_print.hpp"
#include "function_impl.hpp"
using namespace exp_print;

template<size_t Idx, class T>
struct inserter {
	static const int value = Idx;
	using type = T;
};
template<class TL, class Inserter>
struct insert_at_impl
{
	using front = sub_type_list<Inserter::value + 1, TL>;
	using back = experiment::exp_ignore_until<Inserter::value, TL>;
	using type = typename exp_join_a_lot<front,
		exp_list<typename Inserter::type>,
		back>::type;
};
template<class TL, class Inserter>
using insert_at = typename insert_at_impl<TL, Inserter>::type;

using mtl = exp_list<char, int, double, char, char>;
using namespace function_impl;
struct X {};

template<size_t N, class node, class T>
auto insert_node(node n, T const& v)
{
	auto node_maker = [](auto && ...x) {
		return make_element_node(shared_constructor(), x...);
		};

	using new_typelist = insert_at<
		typename node::element_type_list,
		inserter<N, T>>;

	auto maker_binder = exp_bind::bind(
		function_impl::realize_meta<new_typelist>(node_maker));

	auto bind_efb = [&maker_binder](auto& value) {
		maker_binder.bind(value);
		};
	size_t pos{};
	for (; pos <= N; ++pos) {
		do_at(n, bind_efb, pos);
	}
	maker_binder.bind(v);
	for (; pos <= exp_iterator<node>{n}.size() - N; ++pos) {
		do_at(n, bind_efb, pos);
	}
	return maker_binder.apply_func();
}

template<size_t N, class tuple_t, class T>
auto insert_tuple(tuple_t& tp, T const& v)
{
	tuple_iterator ti{ tp };
	auto uwNode = exp_bind::unwrapped_node(ti.first_node);
	auto newTpNode = insert_node<N>(uwNode, v);
	return exp_bind::node_to_tuple(newTpNode);
}

struct super_base {
	virtual void call() {}
	virtual void set(void(*p)(void*)) {}
	virtual void destroy() { delete this; }
};

template<class T>
struct super_inherit:super_base
{
	T* pointer{};
	super_inherit(T* p) noexcept :pointer(p) {};
	void call() {
		(*pointer)._continue();
	}
	void destroy()
	{
		delete pointer;
	}
	~super_inherit()
	{
		//delete pointer;
	}
};
template<class T>
super_inherit(T val) -> super_inherit<T>;

template<class T>
using add_ptr = T*;
using namespace exp_function_series;



class student_score
{
public:
	student_score() {}
	void set_score(int score) { m_score = score; }
	int get_score() { return m_score; }
	void set_name(std::string name) { m_name = name; }
	std::string get_name() { return m_name; }
private:
	int m_score{};
	std::string m_name{};
};

void set_student_score(student_score* ss, auto && link_o, std::string name, int score)
{
	auto [_1/*因为函数返回 void*/, _2/*因为函数返回 void*/, mname, mscore] = link_o(ss, name, score);
	std::println(std::cout, "student: {} , score: {} recorded\n", mname, mscore);
}
using exp_function_series::link_object;


template<class T>
concept vector_type = requires(T a) { is_wrapped_with<decltype(a), std::vector>::value; };

template<class T>
void foo(vector_type<T> A) {};
int main()
{
	
	int a;
	foo(a);

}