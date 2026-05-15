#pragma once
#include<type_traits>
#include<cstdint>
#include<limits>
namespace cppp::unsigned_limits{
    template<std::uintmax_t num,typename ...T>
    struct smallest_fit_in{
        using type = void;
    };
    template<std::uintmax_t num,typename A,typename ...T>
    struct smallest_fit_in<num,A,T...>{
        using type = std::conditional_t<
            num <= static_cast<std::uintmax_t>(std::numeric_limits<A>::max()),
            A,
            typename smallest_fit_in<num,T...>::type
        >;
    };
    template<std::uintmax_t num,typename ...T>
    using smallest_fit_in_t = smallest_fit_in<num,T...>::type;
    template<std::uintmax_t num>
    using smallest_int_fit_t = smallest_fit_in<num,std::uint16_t,std::uint32_t,std::uint64_t>::type;
    template<std::uintmax_t num>
    using smallest_fit_t = smallest_fit_in<num,std::uint8_t,std::uint16_t,std::uint32_t,std::uint64_t>::type;
    template<std::uintmax_t num>
    using fastest_int_fit_t = smallest_fit_in<num,std::uint_fast16_t,std::uint_fast32_t,std::uint_fast64_t>::type;
    template<std::uintmax_t num>
    using fastest_fit_t = smallest_fit_in<num,std::uint_fast8_t,std::uint_fast16_t,std::uint_fast32_t,std::uint_fast64_t>::type;
}
