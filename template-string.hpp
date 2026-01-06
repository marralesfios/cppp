#pragma once
#include<cstdint>
namespace cppp{
    template<std::size_t l>
    struct template_string{
        char arr[l];
        using iterator = char*;
        using const_iterator = const char*;
        constexpr template_string(const char(&a)[l]){
            std::size_t i = l;
            while(i--){
                arr[i] = a[i];
            }
        }
        consteval std::size_t size() const{
            return l-1uz;
        }
        constexpr const char& operator[](std::size_t idx) const{
            return arr[idx];
        }
        constexpr char& operator[](std::size_t idx){
            return arr[idx];
        }
        constexpr char* begin(){
            return arr;
        }
        constexpr char* end(){
            return arr+size();
        }
        constexpr const char* begin() const{
            return arr;
        }
        constexpr const char* end() const{
            return arr+size();
        }
    };
    inline namespace literals{
        template<template_string s>
        consteval decltype(s) operator ""_ts(){
            return s;
        }
    }
}
