#include<utility>
#include<meta>
namespace cppp{
    namespace detail{
        template<std::meta::info fn,typename R,typename ...A>
        constexpr R perform_invoke(A&& ...a){
            return [:fn:](std::forward<A>(a)...);
        }
    }
    template<typename R,typename ...A>
    struct consteval_function{
        std::meta::info _fn;
        consteval consteval_function(std::meta::info inf) : _fn(inf){
            using namespace std::literals;
            if(!is_invocable_type(type_of(inf),{^^A...})) throw std::meta::exception(u8"cppp::consteval_function argument is not correctly invocable"sv,inf);
            if(!is_convertible_type(invoke_result(inf,{^^A...}))) throw std::meta::exception(u8"cppp::consteval_function argument does not return a compatible type"sv,inf);
        }
        template<std::invocable<A...> F> requires(std::convertible_to<std::invoke_result_t<F,A...>,R>)
        consteval consteval_function(F val) : _fn(std::meta::reflect_constant(val)){}
        constexpr R operator()(A ...argv) const{
            return extract<R(*)(A&&...)>(substitute(^^detail::perform_invoke,{reflect_constant(_fn),^^R,^^A...}))(std::forward<A>(argv)...);
        }
    };
}
