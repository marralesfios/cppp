#pragma once
namespace cppp{
    template<typename T>
    T& rtl(T&& ref){
        return static_cast<T&>(ref);
    }
}
