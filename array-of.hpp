#pragma once
#include<cstddef>
#include<array>
namespace cppp{
    template<typename T>
    struct array_of{
        template<std::size_t n>
        using type = std::array<T,n>;
    };
}
