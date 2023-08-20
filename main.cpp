#include<iostream>
#include<functional>
#include"join_list.h"
#include<string>

int foo(int x)
{
	return x;
}

int sum(int x, int y, int z)
{
	return x + y + z;
}

struct X
{
	int foo(int x)
	{
		return x;
	}

	int sum(int x, int y, int z)
	{
		return x + y + z;
	}
};

void print(auto x)
{
	std::cout << x << std::endl;
}
using namespace exp_bind;
int main()
{
	X x;
	auto emw = exp_mem_wrap(x, &X::sum);
	auto efb = bind_mem(emw);

	efb.bind(1);
	efb.bind_a_lot(2, 3);
	print(efb());
	efb[2] = 0;
	print(efb());
}