#pragma once
#include<unordered_map>
#include<unordered_set>
#include<string_view>
#include<string>
#include<map>
#include<set>
namespace cppp{
    namespace detail{
        struct str_hash{
            using is_transparent = void;
            constexpr std::size_t operator()(const std::u8string& v) const noexcept{
                return std::hash<std::u8string>()(v);
            }
            constexpr std::size_t operator()(const std::u8string_view& v) const noexcept{
                return std::hash<std::u8string_view>()(v);
            }
        };
        struct str_equ{
            using is_transparent = void;
            constexpr bool operator()(const std::u8string& lhs,const std::u8string& rhs) const noexcept{
                return lhs == rhs;
            }
            constexpr bool operator()(const std::u8string_view& lhs,const std::u8string& rhs) const noexcept{
                return lhs == rhs;
            }
            constexpr bool operator()(const std::u8string& lhs,const std::u8string_view& rhs) const noexcept{
                return lhs == rhs;
            }
            constexpr bool operator()(const std::u8string_view& lhs,const std::u8string_view& rhs) const noexcept{
                return lhs == rhs;
            }
        };
        struct str_lt{
            using is_transparent = void;
            constexpr bool operator()(const std::u8string& lhs,const std::u8string& rhs) const noexcept{
                return lhs < rhs;
            }
            constexpr bool operator()(const std::u8string_view& lhs,const std::u8string& rhs) const noexcept{
                return lhs < rhs;
            }
            constexpr bool operator()(const std::u8string& lhs,const std::u8string_view& rhs) const noexcept{
                return lhs < rhs;
            }
            constexpr bool operator()(const std::u8string_view& lhs,const std::u8string_view& rhs) const noexcept{
                return lhs < rhs;
            }
        };
    }
    template<typename E>
    using strmap = std::unordered_map<std::u8string,E,detail::str_hash,detail::str_equ>;
    using strset = std::unordered_set<std::u8string,detail::str_hash,detail::str_equ>;
    template<typename E>
    using ordered_strmap = std::map<std::u8string,E,detail::str_lt>;
    using ordered_strset = std::set<std::u8string,detail::str_lt>;
}
