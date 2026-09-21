#pragma once
#include"int.hpp"
#include<bit>
namespace cppp{
    template<typename T>
    constexpr std::uint32_t countl_zero(T v) noexcept{
        return assume_cast<std::uint32_t>(std::countl_zero(v));
    }
    template<typename T>
    constexpr std::uint32_t countr_zero(T v) noexcept{
        return assume_cast<std::uint32_t>(std::countr_zero(v));
    }
}
