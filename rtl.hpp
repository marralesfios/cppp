#pragma once
namespace cppp{
    template<typename T>
    constexpr T& rtl(T&& ref) noexcept{
        return static_cast<T&>(ref);
    }
}
