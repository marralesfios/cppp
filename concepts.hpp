#pragma once
#include<concepts>
namespace cppp{
    template<typename T,typename ...U>
    concept one_of = (... || std::same_as<T,U>);
}
