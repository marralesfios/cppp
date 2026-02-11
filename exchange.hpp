#pragma once
#include<type_traits>
#include<utility>
namespace cppp{
    namespace detail{
        constexpr inline void shl() noexcept{}
        template<typename T,typename U>
        constexpr void shl(T& x,U&& y){
            x = std::forward<U>(y);
        }
        template<typename T,typename U,typename ...Etc> requires(sizeof...(Etc)>0uz)
        constexpr void shl(T& x,U& y,Etc&& ...etc) noexcept(noexcept(x = std::move(y)) && noexcept(shl(y,std::forward<Etc>(etc)...))){
            x = std::move(y);
            shl(y,std::forward<Etc>(etc)...);
        }
    }
    template<typename First,typename ...Rest>
    constexpr First shl(First& first,Rest&& ...rest) noexcept(std::is_nothrow_move_constructible_v<First> && noexcept(detail::shl(first,std::forward<Rest>(rest)...))){
        First old_value{std::move(first)};
        detail::shl(first,std::forward<Rest>(rest)...);
        return old_value;
    }
}
