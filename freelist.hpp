#pragma once
#include"zeroing-field.hpp"
#include<cstdint>
#include<set>
namespace cppp{
    template<typename T>
    class freelist{
        zeroing_field<T,true> _max;
        zeroing_field<T,true> _size;
        std::set<T,std::greater<T>> list;
        public:
            freelist() = default;
            explicit freelist(T v) : _max(v), _size(v){}
            const std::set<T,std::greater<T>>& holes() const{
                return list;
            }
            void clear(){
                list.clear();
                _size.reset();
            }
            void reset(){
                clear();
                _max.reset();
            }
            T size() const{
                return *_size;
            }
            T max_size() const{
                return *_max;
            }
            T allocate(){
                if(list.empty()){
                    if(++*_size > *_max) _max = *_size;
                    return *_size - 1;
                }else{
                    T ind = *list.begin();
                    list.erase(list.begin());
                    return ind;
                }
            }
            bool occupied(T ind) const{
                return ind < *_size && !list.contains(ind);
            }
            void deallocate(T ind){
                list.emplace(ind);
                T ls;
                while(!list.empty()&&(*list.begin())==(ls = *_size-1)){
                    list.erase(list.begin());
                    _size = ls;
                }
            }
    };
}
