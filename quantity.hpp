#pragma once
#include<iostream>
#include<type_traits>
#include<boost/mpl/vector.hpp>
#include<boost/mpl/vector_c.hpp>
#include<boost/mpl/transform.hpp>
#include<boost/mpl/plus.hpp>
#include<boost/mpl/minus.hpp>
#include<boost/mpl/int.hpp>
#include<boost/mpl/equal.hpp>
#include<boost/mpl/placeholders.hpp>

namespace mpl = boost::mpl;
using namespace mpl::placeholders;

typedef mpl::vector_c<int, 1, 0, 0, 0, 0, 0, 0> mass;
typedef mpl::vector_c<int, 0, 1, 0, 0, 0, 0, 0> length;
typedef mpl::vector_c<int, 0, 0, 1, 0, 0, 0, 0> qtime;
typedef mpl::vector_c<int, 0, 0, 0, 1, 0, 0, 0> charge;
typedef mpl::vector_c<int, 0, 0, 0, 0, 1, 0, 0> templature;
typedef mpl::vector_c<int, 0, 0, 0, 0, 0, 1, 0> intensity;
typedef mpl::vector_c<int, 0, 0, 0, 0, 0, 0, 1> mass_of_substance;
typedef mpl::vector_c<int, 0, 1, -2, 0, 0, 0, 0> acceleration;
typedef mpl::vector_c<int, 1, 1, -2, 0, 0, 0, 0> force;


template<class T, class demission>
struct quantity
{
private:
	T m_value;
public:
	explicit quantity(T x)
		:
		m_value(x)
	{}
	template<class T, class Other_Demission>
	quantity(quantity<T, Other_Demission> const& rhs)
		:
		m_value(rhs.value())
	{
		static_assert(mpl::equal<demission, Other_Demission>::type::value, "mismatched demission type");
	}
	T value() const
	{
		return m_value;
	}
};

template<class T, class D>
quantity<T, D>
operator+(quantity<T, D> x, quantity<T, D> y)
{
	return quantity<T, D>(x.value() + y.value());
}

template<class T, class D>
quantity<T, D>
operator-(quantity<T, D> x, quantity<T, D> y)
{
	return quantity<T, D>(x.value() - y.value());
}

struct plus_f
{
	template <class T1, class T2>
	struct apply
	{
		typedef typename mpl::plus<T1, T2>::type type;
	};
};
template<class T, class D1, class D2>
quantity<T, typename mpl::transform<D1, D2, plus_f>::type>
operator*(quantity<T, D1> x, quantity<T, D2> y)
{
	typedef typename mpl::transform<D1, D2, plus_f>::type dim;
	return quantity<T, dim>(x.value() * y.value());
}

struct minus_f
{
	template<class T1, class T2>
	struct apply : mpl::minus<T1, T2> {};
};

template<class T, class D1, class D2>
quantity<T, typename mpl::transform<D1, D2, mpl::minus<_1, _2>>::type>
operator/(quantity<T, D1> x, quantity<T, D2> y)
{
	return quantity<T, typename mpl::transform<D1, D2, mpl::minus<_1, _2>>::type>(x.value() / y.value());
}