#pragma once
#include<cstdint>
#include<utility>
#include<span>
#include"iterable.hpp"
namespace cppp{
    template<typename T>
    class fixed_array{
        std::span<T> buf;
        public:
            fixed_array(std::size_t sz) : buf(new T[sz],sz){}
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
