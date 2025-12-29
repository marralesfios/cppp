#pragma once
#include<type_traits>
#include<utility>
#include<cstdint>
#include<cmath>
#include"type_traits/copy-const.hpp"
namespace cppp{
    template<typename T>
    class view{
        T* m;
        T* e;
        public:
            view(T* m,T* e) : m(m), e(e){}
            view(T* m,std::size_t s) : m(m), e(m+s){}
            view<copy_const_t<T,std::byte>> to_bytes() const{
                using M = copy_const_t<T,std::byte>*;
                return {reinterpret_cast<M>(m),reinterpret_cast<M>(e)};
            }
            T& operator[](std::size_t i) const{
                return m[i];
            }
            T* read(std::size_t n=1uz){
                return std::exchange(m,m+n);
            }
            view<T> subview(std::size_t start,std::size_t len=-1uz) const{
                return view(m+start,std::min(len,size()));
            }
            view<T> slice(std::size_t start,std::size_t stop=-1uz) const{
                return view(m+start,m+std::min(stop,size()));
            }
            T* begin(){
                return m;
            }
            T* end(){
                return e;
            }
            std::size_t size() const{
                return e-m;
            }
    };
    using byte_view = view<std::byte>;
    using frozen_byte_view = view<const std::byte>;
}
