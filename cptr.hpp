#pragma once
#include<concepts>
#include"ptr.hpp"
namespace cppp{
    template<typename T>
    class scptr : public ptr<T>{
        public:
            using ptr<T>::ptr;
            constexpr scptr(T*&& p) noexcept : ptr<T>(p){}
            constexpr scptr(const scptr& cpy) : ptr<T>(cpy?new T(*cpy):nullptr){}
            constexpr scptr& operator=(const scptr& cpy){
                this->reset(cpy?new T(*cpy):nullptr);
                return *this;
            }
    };
    template<typename T>
    class dcptr{
        template<typename U>
        friend class dcptr;
        ptr<T> p;
        void*(*cloner)(const void*);
        template<std::derived_from<T> U>
        dcptr(U* p) : p(p), cloner([](const void* p) -> void* {return new U(*static_cast<const U*>(p));}){}
        T* _clone() const{
            if(const T* ptr=p.get()){
                return static_cast<T*>(cloner(ptr));
            }
            return nullptr;
        }
        public:
            constexpr dcptr() noexcept : p(nullptr), cloner(nullptr){}
            dcptr(const dcptr& other) : p(other._clone()), cloner(other.cloner){}
            constexpr dcptr(dcptr&& other) noexcept : p(std::move(other.p)), cloner(other.cloner){}
            template<std::derived_from<T> U>
            dcptr(const dcptr<U>& other) : p(other._clone()), cloner(other.cloner){}
            template<std::derived_from<T> U>
            constexpr dcptr(dcptr<U>&& other) noexcept : p(std::move(other.p)), cloner(other.cloner){}
            template<typename ...Ca>
            static dcptr<T> construct(Ca&& ...ca){
                return {new T(std::forward<Ca>(ca)...)};
            }
            constexpr T* get() noexcept{
                return p.get();
            }
            constexpr const T* get() const noexcept{
                return p.get();
            }
            constexpr T& operator*() noexcept{
                return *p;
            }
            constexpr const T& operator*() const noexcept{
                return *p;
            }
            constexpr T* operator->() noexcept{
                return p.get();
            }
            constexpr const T* operator->() const noexcept{
                return p.get();
            }
            dcptr& operator=(const dcptr& other){
                p.reset(other._clone());
                cloner = other.cloner;
                return *this;
            }
            constexpr dcptr& operator=(dcptr&& other) noexcept{
                p = std::move(other.p);
                cloner = other.cloner;
                return *this;
            }
            template<std::derived_from<T> U>
            dcptr& operator=(const dcptr<U>& other){
                p.reset(other._clone());
                cloner = other.cloner;
                return *this;
            }
            template<std::derived_from<T> U>
            constexpr dcptr& operator=(dcptr<U>&& other) noexcept{
                p = std::move(other);
                cloner = other.cloner;
                return *this;
            }
            //Destruction handled by ptr, no need for custom dtor.
    };
    template<typename T>
    using cptr = std::conditional_t<std::is_polymorphic_v<T>,dcptr<T>,scptr<T>>;
}
