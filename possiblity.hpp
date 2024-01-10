#include<type_traits>

#include<tuple>
#include<iostream>
namespace possibilities
{
    //需要C++20以上
    //原理是用std::is_constructible猜测类的成员类型
    namespace possibilities_utilities
    {
        struct inits
        {
            template<class T> operator T();
        };
        //这个类型用来试探某种类型是不是类的第N个成员类型
        //由于std::is_constructible隐式转换也会true，所以这里禁止隐式转换以求结果精确
        template<class T> struct convert_forbid
        {
            using type = T;
            template<class U,class En = std::enable_if_t<std::is_same_v<T, U>>> operator U();
        };
        //计算成员数量,生成构造列表
        template<bool, class T, class construct_list, size_t cnt> struct count_size{};
        template<class T, template<class...> class construct_list, size_t cnt, class ...types>
        struct count_size<true, T, construct_list<types...>, cnt>
        {
            static constexpr size_t value = cnt;
            using CTL = construct_list<T, types...>;
        };
        template<class T, template<class ...> class construct_list, size_t cnt, class First, class ...rest>
        struct count_size<false, T, construct_list<First, rest...>, cnt>
        {
            using next = count_size<std::is_constructible_v<T, First, rest...>,T, std::conditional_t<
                std::is_constructible_v<T, First, rest...>, construct_list<First, rest...>, construct_list<rest...>>, 
                (std::is_constructible_v<T, First, rest...> ? cnt : cnt - 1)>;
            static constexpr size_t value =next::value;
            using CTL = typename next::CTL;
        };
        template<size_t N, class pre, class bck, class T> struct replace_at{};

        template<size_t N, template<class...> class pre, template<class...> class bck, class RP, class bck_first, class ...preargs, class... bckargs>
        struct replace_at<N, pre<preargs...>, bck<bck_first, bckargs...>, RP>
        {
            using type = typename replace_at<N - 1, pre<preargs..., bck_first>, bck<bckargs...>, RP>::type;
        };

        template<template<class...> class pre, template<class...> class bck, class RP, class bck_first, class ...preargs, class... bckargs>
        struct replace_at<0, pre<preargs...>, bck<bck_first, bckargs...>, RP>
        {
            using type = pre<preargs..., RP, bckargs...>;
        };

        template<class TL, template<class...> class Name> struct as_ {};
        template<template<class...>class TL, template<class...>class Name, class ...Args> struct as_<TL<Args...>, Name>
        { using type = Name<Args...>; };
        //在列表种选择元素
        template<size_t N, class TL> struct select_element { using type = std::tuple_element_t<N, typename as_<TL, std::tuple>::type>; };
    }
    using namespace possibilities_utilities;

    template<class ...> struct tl {};

    template<class T> using count_size_type = count_size<false, std::decay_t<T>, tl<inits, inits, inits, inits, inits, inits, inits, inits, inits, inits>, 10>;
    template<class T> constexpr size_t size() {
        return count_size_type<T>::value;
    }
    //工具类，测试T的N个成员是否为Mem_type
    template<size_t N, class T, class Mem_type> struct is_constructible_at
    {
        using construct_list = typename count_size_type<T>::CTL;
        using chk_constructible = typename replace_at<N + 1, tl<>, construct_list, convert_forbid<Mem_type>>::type;
        using constructible_type = typename as_<chk_constructible, std::is_constructible>::type;
        static constexpr bool value = constructible_type::value;

    };
    //工具类，用一个列表mem_type_list逐个测试is_constructilbe_at，得到T的第N个成员的类型
    template<bool constructible_, size_t N, class T, class mem_type_list> struct constructible_at_try {};
    template<bool constructible_, size_t N, class T, template<class...> class mem_type_list, class first, class ...Args>
    struct constructible_at_try<constructible_, N, T, mem_type_list<first, Args...>>
    {
        static constexpr bool cstible = is_constructible_at<N, T, first>::value;
        using type = typename constructible_at_try<cstible, N, T,
            std::conditional_t<cstible, mem_type_list<first, Args...>, mem_type_list<Args...>>>::type;
    };

    template<size_t N, class T, template<class...> class mem_type_list, class first, class ...Args> 
    struct constructible_at_try<true, N, T, mem_type_list<first, Args...>> {using type = first;};
    //工具类，生成成员列表
    template<size_t N, class T, class TL, class saved> struct make_construct_list {};
    template<class T, template<class...> class TL, template<class...> class saved, class ...Args, class ...savedArgs> 
    struct make_construct_list<0, T, TL<Args...>, saved<savedArgs...>>
    {
        using current_type = typename constructible_at_try<false, 0, T, TL<Args...>>::type;
        using type = saved<current_type, savedArgs...>;
    };
    template<size_t N, class T, template<class...> class saved, class TL, class ...Args> struct make_construct_list<N, T, TL, saved<Args...>>
    {
        using current_type = typename constructible_at_try<false, N, T, TL>::type;
        using type = typename make_construct_list<N - 1, T, TL, saved<current_type, Args...>>::type;
    };

    template<class T, class Poss> using possibility = typename make_construct_list<possibilities::size<T>() - 1, T, Poss, tl<>>::type;
}
