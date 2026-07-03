#pragma once
#include<utility>
#include<cstdint>
#include<ranges>
#include<array>
#include<cmath>
namespace cppp{
    namespace detail{
        template<typename T,std::size_t>
        using _repeat_for_pack_t = T;
        template<typename T,std::size_t>
        constexpr T _repeat_for_pack_v(T v){
            return v;
        }
        template<typename T,typename Inds>
        class vec_base{};
        template<typename T,std::size_t ...indices>
        class vec_base<T,std::index_sequence<indices...>>{
            protected:
                std::array<T,sizeof...(indices)> m;
                template<typename U> requires(std::constructible_from<T,U>)
                constexpr vec_base(std::array<U,sizeof...(indices)> conv) noexcept : vec_base(static_cast<T>(conv[indices])...){}
            public:
                constexpr vec_base() noexcept = default;
                constexpr vec_base(T v) noexcept : m{_repeat_for_pack_v<T,indices>(v)...}{}
                constexpr vec_base(_repeat_for_pack_t<T,indices>... v) noexcept : m{v...}{}
        };
    }
    template<typename T,std::size_t n>
    class vec : detail::vec_base<T,std::make_index_sequence<n>>{
        using indices_t = std::make_index_sequence<n>;
        using base_t = detail::vec_base<T,indices_t>;
        constexpr std::array<T,n> underlying_array() const noexcept{
            return base_t::m;
        }
        template<typename U,std::size_t m>
        friend class vec;
        public:
            consteval static std::size_t size(){
                return n;
            }
            using base_t::vec_base;
            template<typename U> requires(std::constructible_from<T,U>)
            constexpr explicit(!std::convertible_to<U,T>) vec(vec<U,n> conv) noexcept : base_t(conv.underlying_array()){}
            constexpr T& operator[](std::size_t ind) noexcept{
                return base_t::m[ind];
            }
            constexpr const T& operator[](std::size_t ind) const noexcept{
                return base_t::m[ind];
            }
            constexpr T* data() noexcept{
                return base_t::m.data();
            }
            constexpr const T* data() const noexcept{
                return base_t::m.data();
            }
            constexpr T& x() noexcept{
                return base_t::m[0uz];
            }
            constexpr const T& x() const noexcept{
                return base_t::m[0uz];
            }
            constexpr T& y() noexcept requires(n>1){
                return base_t::m[1uz];
            }
            constexpr const T& y() const noexcept requires(n>1){
                return base_t::m[1uz];
            }
            constexpr T& z() noexcept requires(n>2){
                return base_t::m[2uz];
            }
            constexpr const T& z() const noexcept requires(n>2){
                return base_t::m[2uz];
            }
            constexpr T& w() noexcept requires(n>3){
                return base_t::m[3uz];
            }
            constexpr const T& w() const noexcept requires(n>3){
                return base_t::m[3uz];
            }
            constexpr vec xproj() const noexcept requires(n>1){
                constexpr auto [...indices] = indices_t{};
                return vec((indices == 0uz ? x() : T())...);
            }
            constexpr vec yproj() const noexcept requires(n>1){
                constexpr auto [...indices] = indices_t{};
                return vec((indices == 1uz ? y() : T())...);
            }
            constexpr vec zproj() const noexcept requires(n>2){
                constexpr auto [...indices] = indices_t{};
                return vec((indices == 2uz ? z() : T())...);
            }
            constexpr vec& operator+=(vec other) noexcept{
                template for(constexpr std::size_t index : std::views::indices(n)){
                    base_t::m[index] += other[index];
                }
                return *this;
            }
            constexpr vec operator-() const noexcept{
                constexpr auto [...indices] = indices_t{};
                return {-base_t::m[indices]...};
            }
            constexpr vec& operator-=(vec other) noexcept{
                template for(constexpr std::size_t index : std::views::indices(n)){
                    base_t::m[index] -= other[index];
                }
                return *this;
            }
            constexpr vec& operator*=(vec other) noexcept{
                template for(constexpr std::size_t index : std::views::indices(n)){
                    base_t::m[index] *= other[index];
                }
                return *this;
            }
            constexpr vec& operator/=(vec other) noexcept{
                template for(constexpr std::size_t index : std::views::indices(n)){
                    base_t::m[index] /= other[index];
                }
                return *this;
            }
    };
    #define CPPP_GENERATE_VECTOR_OVERLOADS_WITH_RET(ret,name,...) template<typename T,std::size_t n> constexpr ret name (vec<T,n> lhs,vec<T,n> rhs) noexcept __VA_ARGS__  template<typename T,std::size_t n> constexpr ret name (T lhs,vec<T,n> rhs) noexcept __VA_ARGS__  template<typename T,std::size_t n> constexpr ret name (vec<T,n> lhs,T rhs) noexcept __VA_ARGS__
    #define CPPP_COMMA ,
    #define CPPP_GENERATE_VECTOR_OVERLOADS(name,...) CPPP_GENERATE_VECTOR_OVERLOADS_WITH_RET(vec<T CPPP_COMMA n>,name,__VA_ARGS__)
    namespace detail{
        template<typename T>
        constexpr T index_vec(T v,std::size_t) noexcept{
            return v;
        }
        template<typename T,std::size_t n>
        constexpr T index_vec(vec<T,n> v,std::size_t ind) noexcept{
            return v[ind];
        }
    }
    CPPP_GENERATE_VECTOR_OVERLOADS_WITH_RET(bool,operator==,{
        constexpr auto [...indices] = std::make_index_sequence<n>{};
        return (...&&(index_vec(lhs,indices) == index_vec(rhs,indices)));
    })
    CPPP_GENERATE_VECTOR_OVERLOADS(operator+,{
        return vec<T,n>(lhs) += rhs;
    })
    CPPP_GENERATE_VECTOR_OVERLOADS(operator-,{
        return vec<T,n>(lhs) -= rhs;
    })
    CPPP_GENERATE_VECTOR_OVERLOADS(operator*,{
        return vec<T,n>(lhs) *= rhs;
    })
    CPPP_GENERATE_VECTOR_OVERLOADS(operator/,{
        return vec<T,n>(lhs) /= rhs;
    })
    template<typename T>
    using vec2 = vec<T,2uz>;
    template<typename T>
    using vec3 = vec<T,3uz>;
    template<typename T>
    using vec4 = vec<T,4uz>;
    using fvec2 = vec2<float>;
    using uvec2 = vec2<std::uint32_t>;
    using ivec2 = vec2<std::int32_t>;
    using svec2 = vec2<std::size_t>;
    using fvec3 = vec3<float>;
    using uvec3 = vec3<std::uint32_t>;
    using ivec3 = vec3<std::int32_t>;
    using svec3 = vec3<std::size_t>;
    using fvec4 = vec4<float>;
    using uvec4 = vec4<std::uint32_t>;
    using ivec4 = vec4<std::int32_t>;
    using svec4 = vec4<std::size_t>;
    CPPP_GENERATE_VECTOR_OVERLOADS(fmod,{
        constexpr auto [...indices] = std::make_index_sequence<n>{};
        return {std::fmod(detail::index_vec(lhs,indices),detail::index_vec(rhs,indices))...};
    })
    CPPP_GENERATE_VECTOR_OVERLOADS(max,{
        constexpr auto [...indices] = std::make_index_sequence<n>{};
        return {std::max(detail::index_vec(lhs,indices),detail::index_vec(rhs,indices))...};
    })
    CPPP_GENERATE_VECTOR_OVERLOADS(min,{
        constexpr auto [...indices] = std::make_index_sequence<n>{};
        return {std::min(detail::index_vec(lhs,indices),detail::index_vec(rhs,indices))...};
    })
    #undef CPPP_GENERATE_VECTOR_OVERLOADS
    #undef CPPP_COMMA
    #undef CPPP_GENERATE_VECTOR_OVERLOADS_WITH_RET
}
namespace std{
    template<typename T,std::size_t n>
    struct hash<cppp::vec<T,n>>{
        constexpr size_t operator()(cppp::vec<T,n> v) const noexcept{
            constexpr auto [...indices] = std::make_index_sequence<n>{};
            hash<T> h;
            return (... ^ (h(v[indices])<<(indices&1)));
        }
    };
}
