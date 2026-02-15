#pragma once
#include<cstdint>
#include<utility>
#include"trap.hpp"
namespace cppp{
    template<typename U,typename T>
    consteval U safe_cast(T v){
        consteval_assert(std::in_range<U>(v));
        return static_cast<U>(v);
    }
    inline namespace literals{
        #define CPPP_DEFINE_INTEGRAL_LITERALS(width) consteval std::uint ## width ## _t operator""_u ## width (unsigned long long x){ return safe_cast<std::uint ## width ## _t>(x); } consteval std::int ## width ## _t operator""_s ## width (unsigned long long x){ return safe_cast<std::int ## width ## _t>(x); }
        CPPP_DEFINE_INTEGRAL_LITERALS(8);
        CPPP_DEFINE_INTEGRAL_LITERALS(16);
        CPPP_DEFINE_INTEGRAL_LITERALS(32);
        CPPP_DEFINE_INTEGRAL_LITERALS(64);
        #undef CPPP_DEFINE_INTEGRAL_LITERALS
    }
}
