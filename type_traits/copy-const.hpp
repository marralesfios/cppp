#pragma once
namespace cppp{
    template<typename T,typename U>
    struct copy_const{
        using type = U;
    };
    template<typename T,typename U>
    struct copy_const<const T,U>{
        using type = const U;
    };
    template<typename T,typename U>
    using copy_const_t = copy_const<T,U>::type;
}
