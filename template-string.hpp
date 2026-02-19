#pragma once
#include<algorithm>
#include<cstdint>
#include<string>
namespace cppp{
    namespace detail{
        template<typename CharT,CharT ...ch>
        struct static_char_storage{
            constexpr static CharT mem[]{ch...};
        };
    }
    template<typename CharT,std::size_t l>
    struct basic_template_string{
        CharT arr[l];
        using iterator = CharT*;
        using const_iterator = const CharT*;
        constexpr basic_template_string(const CharT(&a)[l]){
            std::copy_n(a,l,arr);
        }
        constexpr basic_template_string(CharT v) requires(l==2) : arr{v,static_cast<CharT>(0)}{}
        consteval std::size_t size() const{
            return l-1uz;
        }
        template<typename OtherCt>
        constexpr basic_template_string<OtherCt,l> charconv() const{
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
        constexpr operator std::basic_string_view<CharT>() const{
            return sv();
        }
        constexpr operator std::basic_string<CharT>() const{
            return std::basic_string<CharT>(sv());
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
        private:
            template<std::size_t n>
            constexpr basic_template_string(basic_template_string<CharT,n> left,basic_template_string<CharT,l-n+1> right){
                std::copy_n(left.arr,n-1,arr);
                std::copy_n(right.arr,l-n+1,arr+n-1);
            }
            template<typename C,std::size_t u,std::size_t v>
            friend consteval basic_template_string<C,u+v-1uz> operator+(basic_template_string<C,u>,basic_template_string<C,v>);
    };
    template<typename C,std::size_t u,std::size_t v>
    consteval basic_template_string<C,u+v-1uz> operator+(basic_template_string<C,u> lhs,basic_template_string<C,v> rhs){
        return {lhs,rhs};
    }
    template<typename CharT>
    basic_template_string(CharT v) -> basic_template_string<CharT,2uz>;
    template<std::size_t n>
    using c_template_string = basic_template_string<char,n>;
    template<std::size_t n>
    using template_string = basic_template_string<char8_t,n>;
    
    template<typename CharU,basic_template_string ts>
    constexpr std::basic_string_view<CharU> ts_charconv_to_sv = []<std::size_t ...i>(std::index_sequence<i...>){
            return std::basic_string_view<CharU>(detail::static_char_storage<CharU,static_cast<CharU>(ts[i])...>{}.mem,ts.size());
        }(std::make_index_sequence<ts.size()>{});
    inline namespace literals{
        template<basic_template_string s>
        consteval decltype(s) operator ""_ts(){
            return s;
        }
        #define CPPP_DEFINE_CHAR_TS_LITERAL(type) consteval basic_template_string<type,2uz> operator ""_ts(type c){ return {c}; }
        CPPP_DEFINE_CHAR_TS_LITERAL(char);
        CPPP_DEFINE_CHAR_TS_LITERAL(wchar_t);
        CPPP_DEFINE_CHAR_TS_LITERAL(char8_t);
        CPPP_DEFINE_CHAR_TS_LITERAL(char16_t);
        CPPP_DEFINE_CHAR_TS_LITERAL(char32_t);
        #undef CPPP_DEFINE_CHAR_TS_LITERAL
    }
}
