#pragma once
#include"stringify-enum.hpp"
#include"string.hpp"
#include"print.hpp"
#include<type_traits>
#include<concepts>
#include<iostream>
#include<ranges>
namespace cppp{
    inline namespace debugging{
        template<typename T>
        void debug_item(const T* p){
            print<u8"{:p}"_ts>(std::clog,static_cast<const void*>(p));
        }
        inline void debug_item(char c){
            print<u8"'{}'"_ts>(std::clog,c);
        }
        template<std::integral I>
        void debug_item(I i){
            print<u8"{:d}"_ts>(std::clog,i);
        }
        inline void debug_item(cppp::sv s){
            print<u8"{}"_ts>(std::clog,s);
        }
        inline void debug_item(std::string_view s){
            print<u8"{}"_ts>(std::clog,s);
        }
        template<typename E> requires(std::is_enum_v<E>)
        inline void debug_item(E ev){
            using namespace std::literals;
            print<u8"{}"_ts>(std::clog,u8identifier_of(^^E)+u8"::"s+stringify_enum(ev));
        }
        template<typename ...T>
        void debug(T&& ...v){
            template for(constexpr std::size_t i : std::views::indices(sizeof...(T))){
                if constexpr(i){
                    print<u8" "_ts>(std::clog);
                }
                debug_item(std::forward<T...[i]>(v...[i]));
            }
            std::println();
        }
    }
}
