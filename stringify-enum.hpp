#pragma once
#include"string.hpp"
#include<meta>
namespace cppp{
    template<typename EnumT>
    constexpr cppp::sv stringify_enum(EnumT v){
        template for(constexpr auto enumerator : define_static_array(enumerators_of(^^EnumT))){
            if(v == extract<EnumT>(enumerator)){
                return u8identifier_of(enumerator);
            }
        }
        return {};
    }
}
