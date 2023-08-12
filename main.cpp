#include"select_type.hpp"
#include<memory>
#include<iostream>
#include<functional>
#include<tuple>
using m_t_list = exp_list<int, double, char, unsigned, char>;

using first_node_type = element_node<0, m_t_list, std::shared_ptr>;



int main()
{
	auto str_tup = std::make_tuple(std::string("this is "), 2, std::string(" , another integer is "), 10);
	tuple_iterator<decltype(str_tup)> st_iter(str_tup);
	auto& it = st_iter.iterator();
	std::string str{};
	it[1] = 4;
	for (auto i = 0; i < it.size(); ++i)
	{
		it[i] >> std::cout;
	}
	std::cout << std::endl;
	it.reset();

	int d_v{ 1 };

	std::cout << "fetch second integer value:";
	std::cout << fetch_value(d_v, it[3]) << std::endl;
	auto shr_con = []<class Ty, class T>(T & value) { return std::make_shared<Ty>(value); };

	auto ils = make_element_node(shr_con, 1, 2.77, 3);
	exp_iterator<decltype(ils)> ils_it(ils);
	ils_it[1] = 7.93;
	for (auto i = 0; i < ils_it.size(); ++i)
	{
		ils_it[i] >> std::cout << " ";
	}
	
	return 0;
}