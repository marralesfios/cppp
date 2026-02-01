#pragma once
#include<type_traits>
#include<concepts>
#include<string>
namespace cppp{
    using str = std::u8string;
    using sv = std::u8string_view;
    constexpr inline std::string_view cview(sv s){
        return {reinterpret_cast<const char*>(s.data()),s.size()};
    }
    inline std::string tocs(str&& s){
        return std::string(cview(s));
    }
    inline sv uview(std::string_view s){
        return {reinterpret_cast<const char8_t*>(s.data()),s.size()};
    }
    constexpr inline sv uview(const std::string&) = delete;
    inline str tou8(const std::string& s){
        return {s.begin(),s.end()};
    }
    inline str tou8(const std::string_view& s){
        return {s.begin(),s.end()};
    }
    namespace detail{
        template<typename T>
        concept is_u8string = std::same_as<std::remove_cvref_t<T>,cppp::str> || std::same_as<std::remove_cvref_t<T>,cppp::sv>;
    }
    template<typename T>
    std::conditional_t<detail::is_u8string<T>,std::string_view,T&&> forward_to_stdstring(std::type_identity_t<T>& lref){
        if constexpr(detail::is_u8string<T>){
            return cview(lref);
        }else{
            return static_cast<T&&>(lref);
        }
    }
    template<typename T> requires(!std::is_lvalue_reference_v<T>)
    
    
    std::conditional_t<detail::is_u8string<T>,std::string_view,T&&> forward_to_stdstring(std::type_identity_t<T>&& rref){
        if constexpr(detail::is_u8string<T>){
            return cview(rref);
        }else{
            return static_cast<T&&>(rref);
        }
    }
}
