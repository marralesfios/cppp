#pragma once
#include<experimental/propagate_const>
#include<utility>
#include<memory>
namespace cppp{
    template<typename T>
    class ptr : public std::experimental::propagate_const<std::unique_ptr<T>>{
        public:
            using std::experimental::propagate_const<std::unique_ptr<T>>::propagate_const;
            void reset(T* p=nullptr){
                std::experimental::get_underlying(*this).reset(p);
            }
            template<typename ...Ca>
            static ptr<T> construct(Ca&& ...ca){
                return ptr<T>(new T(std::forward<Ca>(ca)...));
            }
    };
}
