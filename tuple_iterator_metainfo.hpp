#pragma once
#include"select_type.hpp"
#ifndef _TUPLE_META_
#define _TUPLE_META_


namespace tuple_preprocess {
	template<class T>
	struct is_const_ref{
		compilet_var bool value = false;
	};
	template<class T>
	struct is_const_ref<const T&> {
		compilet_var bool value = true;
	};

	template<class T>
	constexpr bool is_const_ref_t = is_const_ref<T>::value;
	template<class TP>
	constexpr bool is_tuple_type = is_wrapped_with<TP, std::tuple>::value;
	template<class T>
	using supported_tuple_element_ref = exp_if<is_reference_type<T>::value && !is_const_ref_t<T>, exp_reference_pointer<T>, std::decay_t<T>>;
	template<class TP>
	using preprocess_tuple = typename exp_apply<TP, supported_tuple_element_ref>::type;
}
#endif