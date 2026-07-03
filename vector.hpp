#pragma once
#include<utility>
#include<cstdint>
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
        template<typename T,std::size_t ...indices>
        class vec{
            public:
                consteval static std::size_t size(){
                    return sizeof...(indices);
                }
            private:
                std::array<T,size()> m;
            public:
                constexpr vec() noexcept = default;
                constexpr vec(T v) noexcept : m{_repeat_for_pack_v<T,indices>(v)...}{}
                constexpr vec(_repeat_for_pack_t<T,indices>... v) noexcept : m{v...}{}
                template<typename U> requires(std::constructible_from<T,U>)
                constexpr explicit(!std::convertible_to<U,T>) vec(vec<U,indices...> conv) : vec(static_cast<T>(conv[indices])...){}
                constexpr T& operator[](std::size_t ind) noexcept{
                    return m[ind];
                }
                constexpr const T& operator[](std::size_t ind) const noexcept{
                    return m[ind];
                }
                constexpr T* data() noexcept{
                    return m.data();
                }
                constexpr const T* data() const noexcept{
                    return m.data();
                }
                constexpr T& x() noexcept{
                    return m[0uz];
                }
                constexpr const T& x() const noexcept{
                    return m[0uz];
                }
                constexpr T& y() noexcept requires(size()>1){
                    return m[1uz];
                }
                constexpr const T& y() const noexcept requires(size()>1){
                    return m[1uz];
                }
                constexpr T& z() noexcept requires(size()>2){
                    return m[2uz];
                }
                constexpr const T& z() const noexcept requires(size()>2){
                    return m[2uz];
                }
                constexpr T& w() noexcept requires(size()>3){
                    return m[3uz];
                }
                constexpr const T& w() const noexcept requires(size()>3){
                    return m[3uz];
                }
                constexpr vec xproj() const noexcept requires(size()>1){
                    return vec((indices == 0uz ? x() : T())...);
                }
                constexpr vec yproj() const noexcept requires(size()>1){
                    return vec((indices == 1uz ? y() : T())...);
                }
                constexpr vec zproj() const noexcept requires(size()>2){
                    return vec((indices == 2uz ? z() : T())...);
                }
                constexpr vec& operator+=(vec other) noexcept{
                    (..., (m[indices] += other[indices]));
                    return *this;
                }
                constexpr vec operator-() const noexcept{
                    return {-m[indices]...};
                }
                constexpr vec& operator-=(vec other) noexcept{
                    (..., (m[indices] -= other[indices]));
                    return *this;
                }
                constexpr vec operator*=(vec other) noexcept{
                    (..., (m[indices] *= other[indices]));
                    return *this;
                }
                constexpr vec& operator/=(vec other) noexcept{
                    (..., (m[indices] /= other[indices]));
                    return *this;
                }
                constexpr bool operator==(vec other) const noexcept{
                    return (...&&(m[indices] == other[indices]));
                }
        };
        template<typename T,std::size_t ...indices>
        constexpr vec<T,indices...> operator+(vec<T,indices...> lhs,vec<T,indices...> rhs) noexcept{
            return lhs += rhs;
        }
        template<typename T,std::size_t ...indices>
        constexpr vec<T,indices...> operator-(vec<T,indices...> lhs,vec<T,indices...> rhs) noexcept{
            return lhs -= rhs;
        }
        template<typename T,std::size_t ...indices>
        constexpr vec<T,indices...> operator*(vec<T,indices...> lhs,vec<T,indices...> rhs) noexcept{
            return lhs *= rhs;
        }
        template<typename T,std::size_t ...indices>
        constexpr vec<T,indices...> operator/(vec<T,indices...> lhs,vec<T,indices...> rhs) noexcept{
            return lhs /= rhs;
        }
        template<typename T,typename U>
        struct vec_from_int_seq{};
        template<typename T,std::size_t ...n>
        struct vec_from_int_seq<T,std::index_sequence<n...>>{
            using type = vec<T,n...>;
        };
    }
    template<typename T,std::size_t n>
    using vec = detail::vec_from_int_seq<T,std::make_index_sequence<n>>::type;
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
    template<typename T,std::size_t ...indices>
    detail::vec<T,indices...> fmod(detail::vec<T,indices...> lhs,detail::vec<T,indices...> rhs){
        return {std::fmod(lhs[indices],rhs[indices])...};
    }
    template<typename T,std::size_t ...indices>
    detail::vec<T,indices...> max(detail::vec<T,indices...> lhs,detail::vec<T,indices...> rhs){
        return {std::max(lhs[indices],rhs[indices])...};
    }
    template<typename T,std::size_t ...indices>
    detail::vec<T,indices...> min(detail::vec<T,indices...> lhs,detail::vec<T,indices...> rhs){
        return {std::min(lhs[indices],rhs[indices])...};
    }
}
namespace std{
    template<typename T,std::size_t ...indices>
    struct hash<cppp::detail::vec<T,indices...>>{
        constexpr size_t operator()(cppp::detail::vec<T,indices...> v) const noexcept{
            hash<T> h;
            return (... ^ (h(v[indices])<<(indices&1)));
        }
    };
}
