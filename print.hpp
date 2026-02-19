#pragma once
#include"template-string.hpp"
#include"string.hpp"
#include<print>
namespace cppp{
    template<template_string pat,typename ...T>
    void print(T&& ...v){
        std::print(ts_charconv_to_sv<char,pat>,forward_to_stdstring<T>(v)...);
    }
    template<template_string pat,typename ...T>
    void println(T&& ...v){
        std::println(ts_charconv_to_sv<char,pat>,forward_to_stdstring<T>(v)...);
    }
}
