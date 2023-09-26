#include"exp_repeat.hpp"
#include"exp_print.hpp"
#include <string>
#include "exp_function_binder.hpp"
#include "select_type.hpp"
#include "ex_exp_iterator.hpp"
#include<type_traits>

using namespace std;
struct X
{
	int a{};
	double b{};
	char c{};
	std::string s{};

	auto reflex()
	{
		return make_element_node(shared_constructor(), exp_ref(a), exp_ref(b), exp_ref(c), exp_ref(s));
	}
};



auto main() -> int
{
	auto node = make_element_node(shared_constructor(), "this is it"s, 3);
	string i{};
	exp_iterator it{ node };
	fetch_value(i, it);
}