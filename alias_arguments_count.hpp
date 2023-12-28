#pragma once

namespace alias_c
{
    template<template<class> class F> constexpr size_t alias_argc() { return 1; }
    template<template<class, class> class F> constexpr size_t alias_argc() { return 2; }
    template<template<class, class, class> class F> constexpr size_t alias_argc() { return 3; }
    template<template<class, class, class, class> class F> constexpr size_t alias_argc() { return 4; }
    template<template<class, class, class, class, class> class F> constexpr size_t alias_argc() { return 5; }
    template<template<class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 6; }
    template<template<class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 7; }
    template<template<class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 8; }
    template<template<class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 9; }
    template<template<class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 10; }
    template<template<class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 11; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 12; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 13; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 14; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 15; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 16; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 17; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 18; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 19; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 20; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 21; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 22; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 23; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 24; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 25; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 26; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 27; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 28; }

    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 29; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 30; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 31; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 32; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 33; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 34; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 35; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 36; }
    template<template<class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class, class> class F> constexpr size_t alias_argc() { return 37; }

    template<template<class...> class F, class ...Typs> struct alias_invoke
    {
        using type = F<Typs...>;
    };

}
