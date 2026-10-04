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
    template<typename T,typename Else>
    struct coalesce_void_type{
        using type = T;
    };
    template<typename Else>
    struct coalesce_void_type<void,Else>{
        using type = Else;
    };
    template<typename T,typename Else>
    using coalesce_void_type_t = coalesce_void_type<T,Else>::type;
}
