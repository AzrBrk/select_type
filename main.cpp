#include"select_type.hpp"
#include<memory>
#include<iostream>
#include<functional>
#include<tuple>
using m_t_list = exp_list<int, double, char, unsigned, char>;

using first_node_type = element_node<0, m_t_list, std::shared_ptr>;



int main()
{
	auto str_tup = std::make_tuple(std::string("this is the "), 2, std::string("nd time"));
	tuple_iterator<decltype(str_tup)> st_iter(str_tup);
	auto& it = st_iter.iterator();
	std::string str{};
	for (auto i = 0; i < it.size(); ++i,++it)
	{
		it >> std::cout;
	}
	return 0;
}