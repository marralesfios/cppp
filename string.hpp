#pragma once
#include<algorithm>
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
}
