#pragma once
#include<type_traits>
#include<concepts>
namespace cppp{
    template<typename T>
    concept enumeration = std::is_enum_v<T>;
    template<typename T>
    concept true_unsigned_integral = std::integral<T> && std::is_unsigned_v<T>;
    template<typename T,typename ...U>
    concept one_of = (... || std::same_as<T,U>);
    template<typename T,typename R,typename ...P>
    concept invocable_with_result = std::invocable<T,P...> && std::convertible_to<std::invoke_result_t<T,P...>,R>;
    template<typename T,typename R,typename ...P>
    concept invocable_with_exact_result = std::invocable<T,P...> && std::same_as<std::invoke_result_t<T,P...>,R>;
}
