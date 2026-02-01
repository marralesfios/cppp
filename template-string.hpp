#pragma once
#include<cstdint>
#include<string>
namespace cppp{
    namespace detail{
        template<typename CharT,CharT ...ch>
        struct static_char_storage{
            constexpr static CharT mem[]{ch...};
        };
        template<typename CharT,std::size_t l>
        struct template_string{
            CharT arr[l];
            using iterator = CharT*;
            using const_iterator = const CharT*;
            constexpr template_string(const CharT(&a)[l]){
                std::size_t i = l;
                while(i--){
                    arr[i] = a[i];
                }
            }
            consteval std::size_t size() const{
                return l-1uz;
            }
            template<typename OtherCt>
            constexpr template_string<OtherCt,l> charconv() const{
                OtherCt other_arr[l];
                std::size_t i = l;
                while(i--){
                    other_arr[i] = static_cast<OtherCt>(arr[i]);
                }
                return other_arr;
            }
            constexpr std::basic_string_view<CharT> sv() const{
                return {arr,size()};
            }
            constexpr const CharT& operator[](std::size_t idx) const{
                return arr[idx];
            }
            constexpr CharT& operator[](std::size_t idx){
                return arr[idx];
            }
            constexpr const CharT& front() const{
                return *arr;
            }
            constexpr CharT& front(){
                return *arr;
            }
            constexpr const CharT& back() const{
                return arr[size()-1];
            }
            constexpr CharT& back(){
                return arr[size()-1];
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
    }
    template<std::size_t n>
    using c_template_string = detail::template_string<char,n>;
    template<std::size_t n>
    using template_string = detail::template_string<char8_t,n>;
    
    template<typename CharU,detail::template_string ts>
    constexpr std::basic_string_view<CharU> ts_charconv_to_sv = []<std::size_t ...i>(std::index_sequence<i...>){
            return std::basic_string_view<CharU>(detail::static_char_storage<CharU,static_cast<CharU>(ts[i])...>{}.mem,ts.size());
        }(std::make_index_sequence<ts.size()>{});
    inline namespace literals{
        template<c_template_string s>
        consteval decltype(s) operator ""_ts(){
            return s;
        }
        template<template_string s>
        consteval decltype(s) operator ""_ts(){
            return s;
        }
    }
}
