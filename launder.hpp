#pragma once
#include<memory>
namespace cppp{
    template<typename T>
    T& read_object(std::byte* m){
        return *std::launder(reinterpret_cast<T*>(m));
    }
    template<typename T>
    const T& read_object(const std::byte* m){
        return *std::launder(reinterpret_cast<const T*>(m));
    }
}
