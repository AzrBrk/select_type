#include<iostream>
#include"select_type.hpp"

template<size_t N, class TL>
void print_next(element_<true, N, TL, std::shared_ptr>& e)
{
	std::cout << e.value << std::endl;
}
template<size_t N, class TL>
void print_next(element_ <false, N, TL, std::shared_ptr >& e)
{
	print_list(e);
}

template<bool end_type, size_t N, class TL>
void print_list(element_<end_type, N, TL, std::shared_ptr>& e)
{
	using current_ele_type = element_<end_type, N, TL, std::shared_ptr>;
	std::cout << e.value << " ";
	if constexpr (requires(current_ele_type cet) { cet.next; })
	{
		if (e.next)
			print_next(e.next_element());
	}
	else
		std::cout << std::endl;
}
int main()
{
	auto shr_con = []<class Ty, class T>(Ty, T const& t) { return std::make_shared<Ty>(t); };
	using element_list = element_node<0, exp_list<int, double, char, std::string>, std::shared_ptr>;

	element_list first(1);
	auto second = create_next(first, 2.33, shr_con);
	auto third = create_next(*second, 'k', shr_con);
	auto fourth = create_next(*third, std::string("str"), shr_con);
	//auto fiveth = create_next(*fourth, 1, shr_con);
	print_list(first);
	return 0;
}