#pragma once
#include"template-bind.hpp"
namespace cppp{
    template<template<typename...> typename T,typename ...Args>
    struct but_last{
        using type = T<>;
    };
    template<template<typename...> typename T,typename A0>
    struct but_last<T,A0>{
        using type = T<>;
    };
    template<template<typename...> typename T,typename A0,typename A1,typename ...Rest>
    struct but_last<T,A0,A1,Rest...>{
        using type = but_last<template_bind<T,A0>::template wrapper,A1,Rest...>::type::type;
    };
}
