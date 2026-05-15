#pragma once
#include<concepts>
namespace cppp{
    template<typename T,template<typename...> typename Te>
    struct is_derived_from_instantiation{
        constexpr static bool value = false;
    };
    template<template<typename...> typename Te,typename ...Values,std::derived_from<Te<Values...>> cls>
    struct is_derived_from_instantiation<cls,Te>{
        constexpr static bool value = true;
    };
    template<typename T,template<typename...> typename Te>
    constexpr static bool is_derived_from_instantiation_v = is_derived_from_instantiation::value;
    template<typename T,template<typename...> typename Te>
    concept derived_from_instantiation = is_derived_from_instantiation_v<T,Te>;
}
