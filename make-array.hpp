#pragma once
#include<type_traits>
#include<utility>
#include<array>
namespace cppp{
    template<typename T,typename ...Args>
    constexpr std::array<T,sizeof...(Args)> make_array(Args&& ...args) noexcept(noexcept(std::array<T,sizeof...(Args)>{std::forward<Args>(args)...})){
        return {std::forward<Args>(args)...};
    }
}
