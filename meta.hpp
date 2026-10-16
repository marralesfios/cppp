#pragma once
#include<string>
#include<meta>
namespace cppp{
    template<typename M,typename T>
    consteval std::meta::info reflect_member(M T::* member){
        using namespace std::literals;
        for(std::meta::info item : members_of(^^T,std::meta::access_context::unprivileged())){
            try{
                if(extract<M T::*>(item) == member){
                    return item;
                }
            }catch(...){}
        }
        throw std::meta::exception(u8"reflect_member(): no member corresponding to member pointer"sv,std::meta::reflect_constant(member));
    }
}
