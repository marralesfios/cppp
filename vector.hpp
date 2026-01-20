#pragma once
#include<utility>
#include<cstdint>
#include<array>
namespace cppp{
    namespace detail{
        template<typename T,std::size_t>
        using _repeat_for_pack_t = T;
        template<typename T,std::size_t>
        T _repeat_for_pack_v(T v){
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
                constexpr vec() = default;
                constexpr vec(T v) noexcept: m{_repeat_for_pack_v<T,indices>(v)...}{}
                constexpr vec(_repeat_for_pack_t<T,indices>... v) noexcept : m{v...}{}
                template<typename U> requires(std::constructible_from<T,U>)
                constexpr explicit(!std::convertible_to<U,T>) vec(vec<U,indices...> conv) : vec(static_cast<T>(conv[indices])...){}
                constexpr T& operator[](std::size_t ind) noexcept{
                    return m[ind];
                }
                constexpr const T& operator[](std::size_t ind) const noexcept{
                    return m[ind];
                }
                constexpr T& x(){
                    return m[0uz];
                }
                constexpr const T& x() const{
                    return m[0uz];
                }
                constexpr T& y() requires(size()>1){
                    return m[1uz];
                }
                constexpr const T& y() const requires(size()>1){
                    return m[1uz];
                }
                constexpr T& z() requires(size()>2){
                    return m[2uz];
                }
                constexpr const T& z() const requires(size()>2){
                    return m[2uz];
                }
                constexpr vec& operator+=(vec other){
                    (..., (m[indices] += other[indices]));
                    return *this;
                }
                constexpr vec operator+(this vec lhs,vec rhs){
                    return lhs += rhs;
                }
                constexpr vec operator-() const{
                    return {-m[indices]...};
                }
                constexpr vec& operator-=(vec other){
                    (..., (m[indices] -= other[indices]));
                    return *this;
                }
                constexpr vec operator-(this vec lhs,vec rhs){
                    return lhs -= rhs;
                }
                constexpr vec operator*=(vec other){
                    (..., (m[indices] *= other[indices]));
                    return *this;
                }
                constexpr vec operator*(this vec lhs,vec rhs){
                    return lhs *= rhs;
                }
                constexpr vec& operator/=(vec other){
                    (..., (m[indices] /= other[indices]));
                    return *this;
                }
                constexpr vec operator/(this vec lhs,vec rhs){
                    return lhs /= rhs;
                }
                constexpr vec& operator+=(T scalar){
                    (..., (m[indices] += scalar));
                    return *this;
                }
                constexpr vec operator+(this vec lhs,T scalar){
                    return lhs += scalar;
                }
                constexpr vec& operator-=(T scalar){
                    (..., (m[indices] -= scalar));
                    return *this;
                }
                constexpr vec operator-(this vec lhs,T scalar){
                    return lhs -= scalar;
                }
                constexpr vec& operator*=(T scalar){
                    (..., (m[indices] *= scalar));
                    return *this;
                }
                constexpr vec operator*(this vec lhs,T scalar){
                    return lhs *= scalar;
                }
                constexpr vec& operator/=(T scalar){
                    (..., (m[indices] /= scalar));
                    return *this;
                }
                constexpr vec operator/(this vec lhs,T scalar){
                    return lhs /= scalar;
                }
                constexpr bool operator==(vec other) const{
                    return (...&&(m[indices] == other[indices]));
                }
        };
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
