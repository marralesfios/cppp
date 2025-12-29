#pragma once
#include"template-string.hpp"
#include<type_traits>
#include<concepts>
#include<cstddef>
#include<utility>
#include<cstdint>
#include<climits>
#include<span>
namespace cppp{
    using buffer = std::span<std::byte>;
    template<std::size_t l>
    using staticbuffer = std::span<std::byte,l>;
    using frozenbuffer = std::span<const std::byte>;
    template<std::size_t l>
    using frozenstaticbuffer = std::span<const std::byte,l>;
    template<typename T>
    staticbuffer<sizeof(T)> memory(T* v){
        return staticbuffer<sizeof(T)>{reinterpret_cast<std::byte*>(v),sizeof(T)};
    }
    template<typename T>
    frozenstaticbuffer<sizeof(T)> memory(const T* v){
        return frozenstaticbuffer<sizeof(T)>{reinterpret_cast<const std::byte*>(v),sizeof(T)};
    }
    template<template_string ts>
    frozenstaticbuffer<ts.size()> operator ""_bbuf(){
        return std::as_bytes(std::span<const char,ts.size()>(ts));
    }
    template<typename T>
    concept is_uint = std::same_as<T,std::byte> || (std::is_integral_v<T> && std::is_unsigned_v<T>);
    namespace detail{
        template<typename T>
        struct _expandbytes{};
        template<std::size_t... index>
        struct _expandbytes<std::integer_sequence<std::size_t,index...>>{
            template<typename I>
            static void write(std::byte* memory,I number){
                (..., (
                    memory[index] = static_cast<std::byte>(number >> (index*CHAR_BIT))
                ));
            }
            template<typename I>
            static I read(const std::byte* memory){
                return (... | (
                    static_cast<I>(memory[index]) << (index*CHAR_BIT)
                ));
            }
        };
    }
    template<is_uint I>
    void write(std::byte* memory,I number){
        detail::_expandbytes<std::make_integer_sequence<sizeof(I)>>::write<I>(memory,number);
    }
    template<is_uint I>
    I read(const std::byte* memory){
        return detail::_expandbytes<std::make_integer_sequence<sizeof(I)>>::read<I>(memory);
    }
}
