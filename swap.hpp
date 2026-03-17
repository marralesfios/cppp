#pragma once
#include<utility>
namespace cppp{
    namespace detail{
         // don't be a function, else we may find ourselves via ADL and infinitely recurse
        struct swapper{
            template<typename T,typename U>
            static void operator()(T& u,U& v){
                using std::swap;
                swap(u,v);
            }
        };
    }
    constexpr inline detail::swapper swap;
}
