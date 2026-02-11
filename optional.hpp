#pragma once
#include<type_traits>
#include<utility>
namespace cppp{
    template<typename T>
    class optional{
        bool has_value;
        alignas(T) unsigned char data[sizeof(T)];
        optional(bool) : has_value(true){}
        void _destroy(){
            if(has_value){
                (**this).~T();
            }
        }
        public:
            constexpr optional() noexcept : has_value(false){}
            template<typename ...A>
            optional(std::in_place_t,A&& ...a) noexcept(std::is_nothrow_constructible_v<T,A...>) : has_value(true){
                new(data) T(std::forward<A>(a)...);
            }
            optional(const T& v)
            noexcept(std::is_nothrow_copy_constructible_v<T>)
            requires(std::is_copy_constructible_v<T>)
            : has_value(true){
                new(data) T(std::move(v));
            }
            optional(T&& v)
            noexcept(std::is_nothrow_move_constructible_v<T>)
            requires(std::is_move_constructible_v<T>)
            : has_value(true){
                new(data) T(std::move(v));
            }
            optional(const optional& other)
            noexcept(std::is_nothrow_copy_constructible_v<T>)
            requires(std::is_copy_constructible_v<T>) : has_value(other.has_value){
                if(has_value){
                    new(data) T(*other);
                }
            }
            optional(optional&& other)
            noexcept(std::is_nothrow_move_constructible_v<T>)
            requires(std::is_move_constructible_v<T>) : has_value(other.has_value){
                if(has_value){
                    new(data) T(std::move(*other));
                }
            }
            optional& operator=(const optional& other)
            noexcept(std::is_nothrow_copy_assignable_v<T>&&std::is_nothrow_copy_constructible_v<T>)
            requires(std::is_copy_assignable_v<T>&&std::is_copy_constructible_v<T>){
                if(has_value){
                    **this = *other;
                }else{
                    new(data) T(*other);
                    has_value = true;
                }
                return *this;
            }
            optional& operator=(optional&& other)
            noexcept(std::is_nothrow_move_assignable_v<T>&&std::is_nothrow_move_constructible_v<T>)
            requires(std::is_move_assignable_v<T>&&std::is_move_constructible_v<T>){
                if(has_value){
                    **this = std::move(*other);
                }else{
                    new(data) T(std::move(*other));
                    has_value = true;
                }
                return *this;
            }
            template<typename ...A>
            void emplace(A&& ...a) noexcept(std::is_nothrow_constructible_v<T,A...>){
                _destroy();
                new(data) T(std::forward<A>(a)...);
                has_value = true;
            }
            void reset(){
                _destroy();
                has_value = false;
            }
            T* operator->() noexcept{
                return reinterpret_cast<T*>(data);
            }
            const T* operator->() const noexcept{
                return reinterpret_cast<const T*>(data);
            }
            T& operator*() noexcept{
                return *reinterpret_cast<T*>(data);
            }
            const T& operator*() const noexcept{
                return *reinterpret_cast<const T*>(data);
            }
            T* ptr(){
                return has_value?reinterpret_cast<T*>(data):nullptr;
            }
            const T* ptr() const{
                return has_value?reinterpret_cast<const T*>(data):nullptr;
            }
            constexpr explicit operator bool() const noexcept{
                return has_value;
            }
            ~optional(){
                _destroy();
            }
    };
}
