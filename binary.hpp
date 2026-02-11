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
    inline namespace literals{
        consteval inline std::byte operator ""_b(unsigned long long x){
            return static_cast<std::byte>(x);
        }
        consteval inline std::byte operator ""_b(char c){
            return static_cast<std::byte>(c);
        }
    }
    using buffer = std::span<std::byte>;
    template<std::size_t l>
    using staticbuffer = std::span<std::byte,l>;
    using frozenbuffer = std::span<const std::byte>;
    template<std::size_t l>
    using frozenstaticbuffer = std::span<const std::byte,l>;
    template<typename T>
    staticbuffer<sizeof(T)> memory(T* v) noexcept{
        return staticbuffer<sizeof(T)>{reinterpret_cast<std::byte*>(v),sizeof(T)};
    }
    template<typename T>
    frozenstaticbuffer<sizeof(T)> memory(const T* v) noexcept{
        return frozenstaticbuffer<sizeof(T)>{reinterpret_cast<const std::byte*>(v),sizeof(T)};
    }
    template<template_string ts>
    frozenstaticbuffer<ts.size()> operator ""_bbuf() noexcept{
        return std::as_bytes(std::span<const char,ts.size()>(ts));
    }
    template<typename T>
    concept is_int = std::same_as<T,std::byte> || std::integral<T>;
    namespace detail{
        template<typename T,std::size_t last>
        struct _expandbytes{};
        template<std::size_t... index,std::size_t last>
        struct _expandbytes<std::integer_sequence<std::size_t,index...>,last>{
            template<typename I>
            static void write(std::byte* memory,I number) noexcept{
                if constexpr(!sizeof...(index)){
                    *memory = static_cast<std::byte>(number);
                }else{
                    (..., (
                        memory[index] = static_cast<std::byte>(number >> (index*CHAR_BIT))
                    ));
                    memory[last] = static_cast<std::byte>(number >> (last*CHAR_BIT));
                }
            }
            template<typename I>
            static I read(const std::byte* memory) noexcept{
                I low;
                if constexpr(sizeof...(index)){
                    low = (... | (
                        // must static cast the << result since the lhs may be promoted after our static cast
                        static_cast<I>(static_cast<I>(memory[index]) << (index*CHAR_BIT))
                    ));
                }else{
                    low = static_cast<I>(0);
                }
                if constexpr(std::is_signed_v<I>){
                    return low | static_cast<I>((static_cast<I>(memory[last]&0b0111'1111_b)-static_cast<I>(memory[last]&0b1000'0000_b)) << (last*CHAR_BIT));
                }else{
                    return low | static_cast<I>(static_cast<I>(memory[last]) << (last*CHAR_BIT));
                }
            }
        };
    }
    template<is_int I>
    void write(std::byte* memory,I number) noexcept{
        if constexpr(std::is_signed_v<I>){
            write<std::make_unsigned_t<I>>(memory,static_cast<std::make_unsigned_t<I>>(number));
        }else{
            detail::_expandbytes<std::make_index_sequence<sizeof(I)-1uz>,sizeof(I)-1uz>::template write<I>(memory,number);
        }
    }
    template<is_int I>
    I read(const std::byte* memory) noexcept{
        return detail::_expandbytes<std::make_index_sequence<sizeof(I)-1uz>,sizeof(I)-1uz>::template read<I>(memory);
    }
}
