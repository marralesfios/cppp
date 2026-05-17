#pragma once
#include<utility>
#include<cstddef>
#include<array>
namespace cppp{
    template<typename T>
    class uninitialized{
        alignas(T) std::array<std::byte,sizeof(T)> mem;
        public:
            uninitialized(){}
            template<typename ...A>
            uninitialized(std::in_place_t,A&& ...a){
                emplace(std::forward<A>(a)...);
            }
            template<typename ...A>
            void emplace(A&& ...a){
                new(mem.data()) T(std::forward<A>(a)...);
            }
            void destroy(){
                (*this)->~T();
            }
            const T& operator*() const{
                return *reinterpret_cast<const T*>(mem.data());
            }
            T& operator*(){
                return *reinterpret_cast<T*>(mem.data());
            }
            const T* operator->() const{
                return reinterpret_cast<const T*>(mem.data());
            }
            T* operator->(){
                return reinterpret_cast<T*>(mem.data());
            }
    };
}
