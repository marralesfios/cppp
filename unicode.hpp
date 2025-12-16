#pragma once
#include<locale>
#include<codecvt>
#include<string>
#include"concepts.hpp"
#include"array.hpp"
namespace cppp{
    namespace detail{
        template<typename T>
        concept unichr = one_of<T,char8_t,char16_t,char32_t>;
    }
    template<detail::unichr T,detail::unichr U>
    std::basic_string<U> convert(std::basic_string_view<T> src){
        using namespace std::literals;
        if constexpr(std::same_as<T,U>){
            return std::basic_string<U>(src);
        }else if constexpr(std::same_as<T,char8_t>||std::same_as<U,char8_t>){
            std::mbstate_t state;
            std::basic_string<U> to;
            fixed_array<U> buf{src.size()*sizeof(U)/sizeof(T)};
            std::codecvt_base::result status;
            do{
                status = std::use_facet<std::codecvt_byname<T,U,std::mbstate_t>>(std::locale::classic()).out(state,src.data(),src.end(),buf.crbegin(),toe,tob);
                if(status==std::codecvt_base::result::error) throw std::logic_error("cppp::convert: "s);
            }while(status==std::codecvt_base::partial);
        }
    }
}
