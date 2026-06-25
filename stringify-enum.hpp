#pragma once
#include"string.hpp"
#include<meta>
namespace cppp{
    template<typename EnumT>
    constexpr cppp::sv stringify_enum(EnumT v){
        template for(constexpr auto enumerator : define_static_array(enumerators_of(^^EnumT))){
            if(v == [:enumerator:]){
                return u8identifier_of(enumerator);
            }
        }
        return {};
    }
    template<typename EnumT>
    constexpr std::string_view c_stringify_enum(EnumT v){
        template for(constexpr auto enumerator : define_static_array(enumerators_of(^^EnumT))){
            if(v == [:enumerator:]){
                return identifier_of(enumerator);
            }
        }
        return {};
    }
}
