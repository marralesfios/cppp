#pragma once
#include<type_traits>
#include<algorithm>
#include<utility>
#include<cstddef>
#include<memory>
#include"exchange.hpp"
#include"assert.hpp"
namespace cppp{
    /*
    RAII uninitialized memory block helper
    
    This class is intended to temporarily hold a memory pointer, and deallocate it when it goes out of scope, for use in holding temporary allocations in reallocation functions (e.g. an append function for a dynamic array). You should probably call release() or exchange() in an old allocation if there are no errors.
    
    emplace_at() and admit() do not leave behind constructed objects when they throw exceptions, but when successful they do NOT ensure that the constructed elements are destroyed together with the container if the latter is not released. You should ensure this yourself.
    
    
    This class will probably be obsoleted by (or at least significantly reworked in the presence of) the more generic std::scope_exit helper in LF TS v3 if/when it makes it into the standard.
    */
    template<typename T>
    class uninitialized_memory{
        T* p;
        std::size_t n;
        public:
            constexpr uninitialized_memory(std::size_t n) : p(std::allocator<T>().allocate(n)), n(n){}
            uninitialized_memory(const uninitialized_memory&) = delete;
            constexpr uninitialized_memory(uninitialized_memory&& other) noexcept : p(std::exchange(other.p,nullptr)), n(other.n){}
            constexpr uninitialized_memory& operator=(const uninitialized_memory&) = delete;
            constexpr uninitialized_memory& operator=(uninitialized_memory&& other) noexcept{
                if(p && p != other.p) std::allocator<T>().deallocate(p,n);
                p = std::exchange(other.p,nullptr);
                n = other.n;
                return *this;
            }
            template<typename ...A>
            constexpr T& emplace_at(std::size_t i,A&& ...a){
                new(p + i) T(std::forward<A>(a)...);
                return p[i];
            }
            void destroy_at(std::size_t i) noexcept{
                p[i].~T();
            }
            constexpr void exchange(T*& p2,std::size_t n2) noexcept{
                n = n2;
                std::ranges::swap(p,p2);
            }
            constexpr void admit(T* other,std::size_t n) noexcept(std::is_nothrow_move_constructible_v<T>){
                std::uninitialized_move_n(other,n,p);
            }
            constexpr void admit_and_destroy(T* other,std::size_t n) noexcept(std::is_nothrow_move_constructible_v<T>){
                admit(other,n);
                std::destroy_n(other,n);
            }
            constexpr const T* get() const noexcept{
                return p;
            }
            constexpr T* get() noexcept{
                return p;
            }
            constexpr T* release() noexcept{
                return std::exchange(p,nullptr);
            }
            constexpr ~uninitialized_memory(){
                if(p) std::allocator<T>().deallocate(p,n);
            }
    };
    template<typename T>
    constexpr T* grow(T* mem,std::size_t length,std::size_t capacity,std::size_t tocapacity){
        uninitialized_memory<T> dstbuf{tocapacity};
        dstbuf.admit_and_destroy(mem,length);
        dstbuf.exchange(mem,capacity);
        return mem;
    }
    template<typename T>
    constexpr T* shrink(T* mem,std::size_t length,std::size_t capacity,std::size_t tocapacity){
        std::ranges::destroy(mem+tocapacity,mem+length);
        uninitialized_memory<T> dstbuf{tocapacity};
        dstbuf.admit_and_destroy(mem,tocapacity);
        dstbuf.exchange(mem,capacity);
        return mem;
    }
    template<typename T,typename ...A>
    T& grow_and_emplace_back(T*& mem,std::size_t nfrom,std::size_t nto,A&& ...a){
        uninitialized_memory<T> dstbuf{nto};
        dstbuf.emplace_at(nfrom,std::forward<A>(a)...);
        try{
            dstbuf.admit_and_destroy(mem,nfrom);
        }catch(...){
            dstbuf.destroy_at(nfrom);
            throw;
        }
        dstbuf.exchange(mem,nfrom);
        return mem[nfrom];
    }
    template<typename T,typename ...A>
    T& emplace_back(T*& mem,std::size_t length,std::size_t capacity,std::size_t tocapacity,A&& ...a){
        if(length == capacity){
            return grow_and_emplace_back(mem,length,tocapacity,std::forward<A>(a)...);
        }else{
            new(mem+length) T(std::forward<A>(a)...);
            return mem[length];
        }
    }
}
