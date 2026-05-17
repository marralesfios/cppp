#pragma once
#include"template-string.hpp"
#include"string.hpp"
#include<print>
namespace cppp{
    template<c_template_string pat,typename ...T>
    void print(T&& ...v){
        std::print(pat.sv(),forward_to_stdstring<T>(v)...);
    }
    template<c_template_string pat,typename ...T>
    void println(T&& ...v){
        std::println(pat.sv(),forward_to_stdstring<T>(v)...);
    }
    template<template_string pat,typename ...T>
    void print(T&& ...v){
        println<pat.template charconv<char>()>(std::forward<T>(v)...);
    }
    template<template_string pat,typename ...T>
    void println(T&& ...v){
        println<pat.template charconv<char>()>(std::forward<T>(v)...);
    }
}
