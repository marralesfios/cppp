#pragma once
namespace cppp{
    [[noreturn]] consteval void consteval_trap() noexcept{
        throw 0;
    }
    consteval void consteval_assert(bool v) noexcept{
        if(!v) consteval_trap();
    }
}
