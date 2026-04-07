#pragma once
#include<utility>
#include<cstdint>
namespace cppp{
    template<typename E,typename V,typename Sz=std::uint16_t>
    class small_vector{
        E* m;
        Sz n;
        Sz c;
        void _dealloc(){
            delete[] m;
        }
        protected:
            V v;
            template<typename ...A>
            small_vector(A&& ...a) : m(nullptr), v(std::forward<A>(a)...){}
        public:
            using size_type = Sz;
            T& operator[](Sz i){
                return m[i];
            }
            const T& operator[](Sz i) const{
                return m[i];
            }
            ~small_vector(){
                _dealloc();
            }
    };
}
