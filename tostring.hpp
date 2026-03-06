#pragma once
#include"template-string.hpp"
#include"string.hpp"
#include<format>
namespace cppp{
    template<typename T>
    str to_u8string(T&& v){
        return tou8(std::to_string(std::forward<T>(v)));
    }
    template<template_string pat,typename ...T>
    void format_to(str& dst,T&& ...v){
        std::format_to(std::back_inserter(dst),ts_charconv_to_sv<char,pat>,forward_to_stdstring<T>(v)...);
    }
    template<template_string pat,typename ...T>
    str format(T&& ...v){
        str ret;
        format_to<pat,T...>(ret,std::forward<T>(v)...);
        return ret;
    }
}
