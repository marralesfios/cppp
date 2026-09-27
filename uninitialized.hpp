#pragma once
#include<utility>
#include<cstddef>
#include<array>
namespace cppp{
    #if __cpp_trivial_union >= 202603L
    #error update this code to use trivial unions
    #endif
    template<typename T>
    class uninitialized{
        alignas(T) std::array<std::byte,sizeof(T)> mem;
        public:
            uninitialized() noexcept{}
            template<typename ...A>
            uninitialized(std::in_place_t,A&& ...a) noexcept(noexcept(emplace(std::forward<A>(a)...))){
                emplace(std::forward<A>(a)...);
            }
            template<typename ...A>
            void emplace(A&& ...a) noexcept(noexcept(new(mem.data()) T(std::forward<A>(a)...))){
                new(mem.data()) T(std::forward<A>(a)...);
            }
            void destroy() noexcept{
                (*this)->~T();
            }
            const T* get() const noexcept{
                return reinterpret_cast<const T*>(mem.data());
            }
            T* get() noexcept{
                return reinterpret_cast<T*>(mem.data());
            }
            const T& operator*() const noexcept{
                return *get();
            }
            T& operator*() noexcept{
                return *get();
            }
            const T* operator->() const noexcept{
                return get();
            }
            T* operator->() noexcept{
                return get();
            }
    };
}
