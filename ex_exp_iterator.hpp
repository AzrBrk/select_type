#pragma once
#include"exp_function_series.hpp"
#include "select_type.hpp"
namespace ex_iter
{
	template<class T, class _Node>
	constexpr bool is_in_Element_s = exp_is_one_of<T, typename _Node::element_type_list>::value;
	template<class _Node>
	class ex_exp_iterator : public exp_iterator<_Node>
	{
	public:
		ex_exp_iterator(_Node& no) :exp_iterator<_Node>(no) {}
		template<class T> requires is_in_Element_s<T, _Node> operator T& ()
		{
			exp_reference_pointer<T&> erp{};
			auto bErp = [&erp]<class V>(V& value)
				{
				if constexpr (RP_compatible<decltype(erp),V>)
				{
					erp = exp_ref(value);
				}
				};
			return erp;
		}
	};
}