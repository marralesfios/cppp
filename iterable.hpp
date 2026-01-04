#pragma once
#include<iterator>
namespace cppp{
    template<typename T>
    class [[deprecated("These don't need to be member functions")]] const_iterable : public T{
        public:
            using T::T;
            constexpr T::const_iterator cbegin() const noexcept(noexcept(std::declval<const T&>().begin())){
                return this->begin();
            }
            constexpr T::const_iterator cend() const noexcept(noexcept(std::declval<const T&>().end())){
                return this->end();
            }
    };
    template<typename T>
    class [[deprecated("These don't need to be member functions")]] reverse_iterable : public T{
        public:
            using T::T;
            using reverse_iterator = std::reverse_iterator<typename T::iterator>;
            using const_reverse_iterator = std::reverse_iterator<typename T::const_iterator>;
            constexpr reverse_iterator rbegin() noexcept(noexcept(std::declval<T&>().end())){
                return reverse_iterator(this->end());
            }
            constexpr const_reverse_iterator crbegin() const noexcept(noexcept(std::declval<const T&>().end())){
                return const_reverse_iterator(this->end());
            }
            constexpr reverse_iterator rend() noexcept(noexcept(std::declval<T&>().begin())){
                return reverse_iterator(this->begin());
            }
            constexpr const_reverse_iterator crend() const noexcept(noexcept(std::declval<const T&>().begin())){
                return const_reverse_iterator(this->begin());
            }
    };
}
