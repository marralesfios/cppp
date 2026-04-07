#pragma once
#include<iterator>
namespace cppp{
    template<typename T>
    class container{
        using value_type = T::value_type;
        using const_iterator = T::const_iterator;
        constexpr T& self(){
            return *static_cast<T*>(this);
        }
        constexpr const T& self() const{
            return *static_cast<const T*>(this);
        }
        public:
            constexpr value_type& front(){
                return *self().begin();
            }
            constexpr const_iterator cbegin() const{
                return self().begin();
            }
            constexpr const value_type& front() const{
                return *cbegin();
            }
            constexpr value_type& back(){
                return *std::prev(self().end());
            }
            constexpr const_iterator cend() const{
                return self().end();
            }
            constexpr const value_type& back() const{
                return *std::prev(cend());
            }
            constexpr T& operator[](std::size_t i) const{
                return *std::next(begin(),i);
            }
    };
}
