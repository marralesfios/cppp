#pragma once
#include<type_traits>
namespace cppp{
    template<auto& Fref> requires(std::is_function_v<std::remove_reference_t<decltype(Fref)>>)
    class static_functor{
        using fptr_t = std::remove_reference_t<decltype(Fref)>*;
        public:
            consteval operator fptr_t() noexcept{
                return &Fref;
            }
    };
}
