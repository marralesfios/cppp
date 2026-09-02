#pragma once
#include<type_traits>
#include<utility>
#include<ranges>
namespace cppp{
    template<typename First,typename ...T>
    constexpr First shl(First&& first,T&& ...items){
        First old_value{std::move(first)};
        if constexpr(sizeof...(T)){
            old_value = std::move(items...[0uz]);
            template for(constexpr std::size_t i : std::views::indices(sizeof...(T)-1uz)){
                items...[i] = std::move(items...[i+1uz]);
            }
        }
        return old_value;
    }
}
