#pragma once
#include<type_traits>
#include<concepts>
#include<cstdint>
#include<cstddef>
#include<utility>
#include<limits>
#include"trap.hpp"
namespace cppp{
    namespace detail{
        template<typename T>
        struct convert{
            using type = T;
        };
        template<typename T> requires(std::is_enum_v<T>)
        struct convert<T>{
            using type = std::underlying_type_t<T>;
        };
        template<typename T>
        using convert_t = convert<T>::type;
        
        template<typename T>
        struct promote{
            using type = T;
        };
        template<>
        struct promote<char>{
            using type = short;
        };
        template<>
        struct promote<unsigned char>{
            using type = unsigned short;
        };
        template<typename T>
        using promote_t = promote<T>::type;
        
        template<typename T>
        using convert_and_promote_t = promote_t<convert_t<T>>;
    }
    #define CPPP_DEFINE_EXTENDED_CMPFN(name) template<typename T,typename U> constexpr bool cmp_ ## name(T u,U v) noexcept{ return std::cmp_ ## name(static_cast<detail::convert_and_promote_t<T>>(u),static_cast<detail::convert_and_promote_t<U>>(v));}

    CPPP_DEFINE_EXTENDED_CMPFN(equal);
    CPPP_DEFINE_EXTENDED_CMPFN(not_equal);
    CPPP_DEFINE_EXTENDED_CMPFN(less);
    CPPP_DEFINE_EXTENDED_CMPFN(greater);
    CPPP_DEFINE_EXTENDED_CMPFN(less_equal);
    CPPP_DEFINE_EXTENDED_CMPFN(greater_equal);
    #undef CPPP_DEFINE_EXTENDED_CMPFN

    template<typename U,typename T>
    constexpr bool in_range(T v) noexcept{
        // numeric_limits doesn't work on enums, so convert U
        using cU_t = detail::convert_t<U>;
        // parentheses to suppress ADL, which erroneously finds std::cmp_* for T = std::byte
        return (cmp_greater_equal)(v,std::numeric_limits<cU_t>::min()) && (cmp_less_equal)(v,std::numeric_limits<cU_t>::max());
    }
    
    template<typename U,typename T>
    consteval U safe_cast(T v){
        consteval_assert(in_range<U>(v));
        return static_cast<U>(v);
    }
    inline namespace literals{
        consteval std::byte operator""_b(unsigned long long x){
            return safe_cast<std::byte>(x);
        }
        consteval std::byte operator""_b(char c){
            return safe_cast<std::byte>(c);
        }
        #define CPPP_DEFINE_INTEGRAL_LITERALS(width) consteval std::uint ## width ## _t operator""_u ## width (unsigned long long x){ return safe_cast<std::uint ## width ## _t>(x); } consteval std::int ## width ## _t operator""_s ## width (unsigned long long x){ return safe_cast<std::int ## width ## _t>(x); }
        CPPP_DEFINE_INTEGRAL_LITERALS(8);
        CPPP_DEFINE_INTEGRAL_LITERALS(16);
        CPPP_DEFINE_INTEGRAL_LITERALS(32);
        CPPP_DEFINE_INTEGRAL_LITERALS(64);
        #undef CPPP_DEFINE_INTEGRAL_LITERALS
    }
}
