#pragma once
#include<cstddef>
#include<utility>
#include<ranges>
#include<memory>
#include"memory.hpp"
namespace cppp{
    template<typename T>
    class fixed_array{
        T* buf;
        std::size_t len;
        constexpr void _destroy() noexcept{
            if(buf){
                std::destroy_n(buf,len);
                std::allocator<T>().deallocate(buf,len);
            }
        }
        public:
            template<std::ranges::sized_range R>
            constexpr fixed_array(std::from_range_t,R&& ran) : buf(std::allocator<T>().allocate(std::ranges::size(ran))), len(std::ranges::size(ran)){
                try{
                    std::ranges::uninitialized_copy(ran,begin());
                }catch(...){
                    std::allocator<T>().deallocate(buf,len);
                    throw;
                }
            }
            fixed_array(std::initializer_list<T> il) : fixed_array(std::from_range,il){}
            fixed_array(uninitialized_memory<T>&& mem) : buf(mem.release()), len(mem.size()){}
            fixed_array(std::size_t sz) : buf(std::allocator<T>().allocate(sz)), len(sz){
                try{
                    std::ranges::uninitialized_value_construct(buf);
                }catch(...){
                    std::allocator<T>().deallocate(buf,len);
                    throw;
                }
            }
            fixed_array(const fixed_array&) = delete;
            fixed_array(fixed_array&& other) noexcept : buf(std::exchange(other.buf,nullptr)), len(std::exchange(other.len,0uz)){}
            fixed_array& operator=(const fixed_array&) = delete;
            constexpr fixed_array& operator=(fixed_array&& other) noexcept{
                if(this != &other){
                    _destroy();
                    buf = std::exchange(other.buf,nullptr);
                    len = other.len;
                }
                return *this;
            }
            constexpr T& operator[](std::size_t i) noexcept{
                return buf[i];
            }
            constexpr const T& operator[](std::size_t i) const noexcept{
                return buf[i];
            }
            constexpr T* data() noexcept{
                return buf;
            }
            constexpr const T* data() const noexcept{
                return buf;
            }
            constexpr std::size_t size() const noexcept{
                return len;
            }
            constexpr bool empty() const noexcept{
                return !len;
            }
            using iterator = T*;
            using const_iterator = const T*;
            constexpr T* begin() noexcept{
                return buf;
            }
            constexpr const T* begin() const noexcept{
                return buf;
            }
            constexpr T* end() noexcept{
                return buf+len;
            }
            constexpr const T* end() const noexcept{
                return buf+len;
            }
            constexpr ~fixed_array(){
                _destroy();
            }
    };
}
