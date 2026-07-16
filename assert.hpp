#pragma once
#include<source_location>
#include<iostream>
#include"string.hpp"
namespace cppp{
    namespace detail{
        template<std::size_t n>
        std::u8string_view view_u8array(const char8_t(&arr)[n]){
            return std::u8string_view(arr,n);
        }
    }
    constexpr inline void assert_true(bool value,std::u8string_view text,std::source_location where=std::source_location::current()){
        if(!value){
            using namespace std::literals;
            std::cerr << "Assertion '"sv << cview(text) << "' failed in function "sv << where.function_name() << "at "sv << where.file_name() << ':' << where.line() << ':' << where.column() << '\n';
            std::abort();
        }
    }
}

#ifdef NDEBUG
#include<utility>
#if __has_cpp_attribute(assume)
#define CPPP_ASSERT(...) [[assume(__VA_ARGS__)]]
#else
#define CPPP_ASSERT(...) do{ if(!(__VA_ARGS__)) std::unreachable(); }while(0)
#endif
namespace cppp{
    [[noreturn]] constexpr inline void unreachable() noexcept{
        std::unreachable();
    }
}
#else
#define CPPP_ASSERT(...) ::cppp::assert_true((__VA_ARGS__),::cppp::detail::view_u8array(u8 ## #__VA_ARGS__))

namespace cppp{
    [[noreturn]] constexpr inline void unreachable(std::source_location where=std::source_location::current()) noexcept{
        using namespace std::literals;
        std::cerr << "Unreachable executed in function "sv << where.function_name() << "at "sv << where.file_name() << ':' << where.line() << ':' << where.column() << '\n';
        std::abort();
    }
}
#endif
