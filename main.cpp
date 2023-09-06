#include"exp_function_binder.hpp"
#include"join_list.h"
#include"exp_function_series.hpp"

constexpr auto _shift = true;

void print(auto&& x, bool shift = false)
{
	std::cout << x;
	if (shift) std::cout << std::endl;
}

void print(auto &&...x)
{
	(print(x), ...);
}

void print_as_line(auto &&...x)
{
	print(x...);
	print('\n');
}

void print_node(auto& node)
{
	exp_iterator iter{ node };
	for (auto i = 0; i < iter.size(); ++i)
	{
		print(iter[i]);
		print(' ');
	}
}

void print_tuple(auto& _tuple)
{
	tuple_iterator tpiter{ _tuple };
	for (auto i = 0; i < tpiter.iterator().size(); ++i)
	{
		print(tpiter.iterator()[i],' ');
	}
}
void print_type(auto&& x)
{
	print(typeid(x).name(), _shift);
}
char foo(char c, int a) { return c + a; }
int foo2(int a, int b) { 
	print((char)('A'+ a + b), ' ');
	return a + b; 
}
int foo3(int a, int b, int c) { 
	print((char)('a' + a + b + c), ' ');
	return a + b + c;
};
void foov(int a) { print(++a); }

using namespace exp_bind;
using namespace exp_function_series;
struct X
{
	char foo(char c, int a) { 
		for (auto i = 0; i < a; ++i)
		{
			print(c);
		}
		print('!', _shift); 
		return c + a; 
	}
	int foo2(int a, int b) {
		for(auto i = 0; i< a+b;++i)
		{
			print("°¡");
		}
		print('!', _shift);
		return a + b;
	}
	int foo3(int a, int b, int c) {
		for (auto i = 0; i < a + b + c; ++i)
		{
			print("°¡");
		}
		print('!', _shift);
		return a + b + c;
	};
	void foov(int a) { print(++a); }

};
#define _MF(Class, Fun) &##Class##::##Fun
template<class T, class ...F>
auto object_self_move(T& obj, F ...f)
{
	auto make_bind = [&obj](auto&& mf) {return exp_bind::bind(obj, mf); };
	return link_efb(make_bind(f)...);
}
auto main() -> int
{
	{
		X x{};
		auto fl = object_self_move(x, _MF(X,foo), _MF(X, foo2), _MF(X, foo3));
		series_bind(fl, 'A', 1, 2, 3, 4, 5, 6);
		print_type(fl);
		fl._stop_at(1);
		fl.bind_last_return();
		fl[1]._continue();
		print_tuple(fl._return());
	}

	
}