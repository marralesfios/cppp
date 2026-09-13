#pragma once
#include<type_traits>
#include<cstddef>
#include"assert.hpp"
namespace cppp{
    template<typename T,std::size_t n,std::size_t align=0uz>
    class potentially_empty_array{
        alignas(align) T array[n];
        public:
            consteval static std::size_t size() noexcept{
                return n;
            }
            consteval static bool empty() noexcept{
                return false;
            }
            constexpr void fill(const T& v) noexcept(std::is_nothrow_copy_assignable_v<T>){
                for(std::size_t i=0uz;i<n;++i){
                    array[i] = v;
                }
            }
            constexpr T* data() noexcept{
                return array;
            }
            constexpr const T* data() const noexcept{
                return array;
            }
            constexpr T& operator[](std::size_t i) noexcept{
                return array[i];
            }
            constexpr const T& operator[](std::size_t i) const noexcept{
                return array[i];
            }
    };
    template<typename T,std::size_t align>
    class potentially_empty_array<T,0uz,align>{
        public:
            consteval static std::size_t size() noexcept{
                return 0uz;
            }
            consteval static bool empty() noexcept{
                return true;
            }
            constexpr void fill(const T&) noexcept{}
            constexpr T* data() noexcept{
                return nullptr;
            }
            constexpr const T* data() const noexcept{
                return nullptr;
            }
            constexpr T& operator[](std::size_t) noexcept{
                unreachable();
            }
            constexpr const T& operator[](std::size_t) const noexcept{
                unreachable();
            }
    };
}
