#pragma once
#include<cstdint>
#include<utility>
#include<ranges>
#include<span>
#include"iterable.hpp"
namespace cppp{
    constexpr inline struct value_init_tag_t{} value_init_tag;
    template<typename T>
    class fixed_array{
        std::span<T> buf;
        public:
            template<std::ranges::sized_range R>
            fixed_array(R&& ran) : fixed_array(std::ranges::size(ran)){
                std::ranges::copy(ran,begin());
            }
            // don't take a std::span or we may conflict with the above
            fixed_array(T* m,std::size_t n) : buf(m,n){}
            fixed_array(std::size_t sz) : buf(new T[sz],sz){}
            fixed_array(std::size_t sz,value_init_tag_t) : buf(new T[sz](),sz){}
            template<typename ...C>
            fixed_array(std::size_t sz,std::in_place_t,C&& ...t) : buf(new T[sz](std::forward<T>(t)...),sz){}
            fixed_array(const fixed_array&) = delete;
            fixed_array(fixed_array&& other) noexcept : buf(std::exchange(other.buf,std::span<T>())){}
            fixed_array& operator=(const fixed_array&) = delete;
            fixed_array& operator=(fixed_array&& other) noexcept{
                buf = std::exchange(other.buf,std::span<T>());
                return *this;
            }
            T& operator[](std::size_t i) noexcept{
                return buf[i];
            }
            const T& operator[](std::size_t i) const noexcept{
                return buf[i];
            }
            T* data() noexcept{
                return buf.data();
            }
            const T* data() const noexcept{
                return buf.data();
            }
            std::size_t size() const noexcept{
                return buf.size();
            }
            T* release() noexcept{
                T* m = data();
                buf = {};
                return m;
            }
            using iterator = T*;
            using const_iterator = const T*;
            T* begin() noexcept{
                return buf.data();
            }
            const T* begin() const noexcept{
                return buf.data();
            }
            T* end() noexcept{
                return buf.data()+buf.size();
            }
            const T* end() const noexcept{
                return buf.data()+buf.size();
            }
            ~fixed_array(){
                delete[] buf.data();
            }
    };
}
