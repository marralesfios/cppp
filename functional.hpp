#pragma once
#include"type-traits.hpp"
#include<functional>
#include<concepts>
#include<utility>
#include<tuple>
namespace cppp{
    template<typename T,typename ...A>
    concept stateless_functor = requires(A&&... a){
        static_cast<const T&>(T{})(std::forward<A>(a)...);
    };
    template<typename T,typename ...A> requires(stateless_functor<T,A...>)
    constexpr inline bool stateless_nothrow_invocable = noexcept(static_cast<const T&>(T{})(std::declval<A&&>()...));
    template<typename T,typename ...A> requires(stateless_functor<T,A...>)
    using stateless_invoke_result = decltype(static_cast<const T&>(T{})(std::declval<A&&>()...));
    template<typename T,typename ...A> requires(stateless_functor<T,A...>)
    constexpr decltype(auto) invoke_stateless(A&& ...a) noexcept(stateless_nothrow_invocable<T,A...>){
        return static_cast<const T&>(T{})(std::forward<A>(a)...);
    }
    namespace detail{
        template<typename T,typename ...A>
        constexpr stateless_invoke_result<T,A...>(&refer_to_stateless_functor())(A...) noexcept{
            if constexpr(requires{
                {T::operator()} -> std::convertible_to<stateless_invoke_result<T,A...>(&)(A...)>;
            }){
                return T::operator();
            }else{
                return *[](A... a) static noexcept(stateless_nothrow_invocable<T,A...>) -> stateless_invoke_result<T,A...>{
                    return invoke_stateless<T>(static_cast<A&&>(a)...);
                };
            }
        }
    }
    template<typename T,typename ...A> requires(stateless_functor<T,A...>)
    constexpr stateless_invoke_result<T,A...>(&reference_to_stateless_functor)(A...) = detail::refer_to_stateless_functor<T,A...>();
    template<typename T,typename ...A> requires(stateless_functor<T,A...>)
    constexpr stateless_invoke_result<T,A...>(&argument_forwarding_reference_to_stateless_functor)(A&&...) = *[](A&& ...a) static noexcept(stateless_nothrow_invocable<T,A...>) -> stateless_invoke_result<T,A...>{
        return invoke_stateless<T>(std::forward<A>(a)...);
    };
    namespace detail{
        template<typename T>
        using clref_if_none = std::conditional_t<std::is_reference_v<T>,T,const T&>;
        template<typename F,typename T>
        struct unary_specify_or_deduce_parameters_functor{
            constexpr static decltype(auto) operator()(clref_if_none<T>&& val) noexcept(stateless_nothrow_invocable<F,clref_if_none<T>>){
                return invoke_stateless<F>(std::forward<clref_if_none<T>>(val));
            }
        };
        template<typename F>
        struct unary_specify_or_deduce_parameters_functor<F,void>{
            template<typename T>
            constexpr static decltype(auto) operator()(T&& val) noexcept(stateless_nothrow_invocable<F,T>){
                return invoke_stateless<F>(std::forward<T>(val));
            }
        };
        template<typename F,typename T,typename U>
        struct binary_specify_or_deduce_parameters_functor{
            constexpr static decltype(auto) operator()(clref_if_none<T>&& lhs,clref_if_none<U>&& rhs) noexcept(stateless_nothrow_invocable<F,T,U>){
                return invoke_stateless<F>(std::forward<clref_if_none<T>>(lhs),std::forward<clref_if_none<U>>(rhs));
            }
        };
        template<typename F,typename U>
        struct binary_specify_or_deduce_parameters_functor<F,void,U>{
            template<typename T>
            constexpr static decltype(auto) operator()(T&& lhs,clref_if_none<U> rhs) noexcept(stateless_nothrow_invocable<F,T,clref_if_none<U>>){
                return invoke_stateless<F>(std::forward<T>(lhs),std::forward<clref_if_none<U>>(rhs));
            }
        };
        template<typename F,typename T>
        struct binary_specify_or_deduce_parameters_functor<F,T,void>{
            template<typename U>
            constexpr static decltype(auto) operator()(clref_if_none<T> lhs,U&& rhs) noexcept(stateless_nothrow_invocable<F,clref_if_none<T>,U>){
                return invoke_stateless<F>(std::forward<clref_if_none<T>>(lhs),std::forward<U>(rhs));
            }
        };
        template<typename F>
        struct binary_specify_or_deduce_parameters_functor<F,void,void>{
            template<typename T,typename U>
            constexpr static decltype(auto) operator()(T&& lhs,U&& rhs) noexcept(stateless_nothrow_invocable<F,T,U>){
                return invoke_stateless<F>(std::forward<T>(lhs),std::forward<U>(rhs));
            }
        };
    }
    template<typename T=void,typename U=T>
    using plus = detail::binary_specify_or_deduce_parameters_functor<std::plus<void>,T,U>;
    template<typename T=void,typename U=T>
    using minus = detail::binary_specify_or_deduce_parameters_functor<std::minus<void>,T,U>;
    template<typename T=void,typename U=T>
    using multiplies = detail::binary_specify_or_deduce_parameters_functor<std::multiplies<void>,T,U>;
    template<typename T=void,typename U=T>
    using divides = detail::binary_specify_or_deduce_parameters_functor<std::divides<void>,T,U>;
    template<typename T=void,typename U=T>
    using modulus = detail::binary_specify_or_deduce_parameters_functor<std::modulus<void>,T,U>;
    template<typename T=void>
    using negate = detail::unary_specify_or_deduce_parameters_functor<std::negate<void>,T>;
    template<typename T=void,typename U=T>
    using equal_to = detail::binary_specify_or_deduce_parameters_functor<std::equal_to<void>,T,U>;
    template<typename T=void,typename U=T>
    using not_equal_to = detail::binary_specify_or_deduce_parameters_functor<std::not_equal_to<void>,T,U>;
    template<typename T=void,typename U=T>
    using greater = detail::binary_specify_or_deduce_parameters_functor<decltype([]<typename P,typename Q>(P&& lhs,Q&& rhs)static noexcept(noexcept(std::forward<P>(lhs) > std::forward<Q>(rhs))) -> decltype(auto) {return std::forward<P>(lhs) > std::forward<Q>(rhs);}),T,U>;
    // applies the total ordering for pointers
    template<typename T=void,typename U=T>
    using std_greater = detail::binary_specify_or_deduce_parameters_functor<std::greater<void>,T,U>;
    template<typename T=void,typename U=T>
    using less = detail::binary_specify_or_deduce_parameters_functor<decltype([]<typename P,typename Q>(P&& lhs,Q&& rhs)static noexcept(noexcept(std::forward<P>(lhs) < std::forward<Q>(rhs))) -> decltype(auto) {return std::forward<P>(lhs) < std::forward<Q>(rhs);}),T,U>;
    // applies the total ordering for pointers
    template<typename T=void,typename U=T>
    using std_less = detail::binary_specify_or_deduce_parameters_functor<std::less<void>,T,U>;
    template<typename T=void,typename U=T>
    using greater_equal = detail::binary_specify_or_deduce_parameters_functor<decltype([]<typename P,typename Q>(P&& lhs,Q&& rhs)static noexcept(noexcept(std::forward<P>(lhs) >= std::forward<Q>(rhs))) -> decltype(auto) {return std::forward<P>(lhs) >= std::forward<Q>(rhs);}),T,U>;
    // applies the total ordering for pointers
    template<typename T=void,typename U=T>
    using std_greater_equal = detail::binary_specify_or_deduce_parameters_functor<std::greater_equal<void>,T,U>;
    template<typename T=void,typename U=T>
    using less_equal = detail::binary_specify_or_deduce_parameters_functor<decltype([]<typename P,typename Q>(P&& lhs,Q&& rhs)static noexcept(noexcept(std::forward<P>(lhs) <= std::forward<Q>(rhs))) -> decltype(auto) {return std::forward<P>(lhs) <= std::forward<Q>(rhs);}),T,U>;
    // applies the total ordering for pointers
    template<typename T=void,typename U=T>
    using std_less_equal = detail::binary_specify_or_deduce_parameters_functor<std::less_equal<void>,T,U>;
    template<typename T=void,typename U=T>
    using logical_and = detail::binary_specify_or_deduce_parameters_functor<std::logical_and<void>,T,U>;
    template<typename T=void>
    using logical_not = detail::unary_specify_or_deduce_parameters_functor<std::logical_not<void>,T>;
    template<typename T=void,typename U=T>
    using logical_or = detail::binary_specify_or_deduce_parameters_functor<std::logical_or<void>,T,U>;
    template<typename T=void,typename U=T>
    using bit_and = detail::binary_specify_or_deduce_parameters_functor<std::bit_and<void>,T,U>;
    template<typename T=void,typename U=T>
    using bit_or = detail::binary_specify_or_deduce_parameters_functor<std::bit_or<void>,T,U>;
    template<typename T=void,typename U=T>
    using bit_xor = detail::binary_specify_or_deduce_parameters_functor<std::bit_xor<void>,T,U>;
    template<typename T=void>
    using bit_not = detail::unary_specify_or_deduce_parameters_functor<std::bit_not<void>,T>;
    
    template<typename T=void>
    using dereferences = detail::unary_specify_or_deduce_parameters_functor<decltype([]<typename P>(P&& val)static noexcept(noexcept(*std::forward<P>(val))) -> decltype(auto) {return *std::forward<P>(val);}),T>;
    template<typename T=void>
    using addresses = detail::unary_specify_or_deduce_parameters_functor<decltype([]<typename P>(P&& val)static noexcept(noexcept(&std::forward<P>(val))) -> decltype(auto) {return &std::forward<P>(val);}),T>;
    
    template<typename T,typename ...A> requires(stateless_functor<T,A...>)
    struct with_parameters{
        constexpr static decltype(auto) operator()(A&&... a) noexcept(stateless_nothrow_invocable<T,A...>){
            return invoke_stateless<T>(std::forward<A>(a)...);
        }
    };
    namespace detail{
        #if __cpp_trivial_union >= 202603L
        #warning we got trivial unions, just use the one from <uninitialized>
        #endif
        template<typename T>
        struct constexpr_uninitialized{
            union{
                T member;
            };
            constexpr constexpr_uninitialized() noexcept{}
            constexpr ~constexpr_uninitialized() noexcept{}
        };
    }
    template<typename T,typename ...A>
    concept requires_temporary_storage_when_called_with = !std::is_void_v<typename T::template temporary_storage_type<A...>>;
    template<typename T,typename ...A>
    struct temporary_storage_type_of{
        using type = void;
    };
    template<typename T,typename ...A> requires(requires_temporary_storage_when_called_with<T,A...>)
    struct temporary_storage_type_of<T,A...>{
        using type = T::template temporary_storage_type<A...>;
    };
    template<typename T,typename ...A>
    using temporary_storage_type_of_t = temporary_storage_type_of<T,A...>::type;
    template<typename T,typename ...A>
    concept stateless_functor_with_possible_temporary_storage = (requires_temporary_storage_when_called_with<T,A...> ? stateless_functor<T,temporary_storage_type_of_t<T,A...>,A...> : stateless_functor<T,A...>);
    namespace detail{
        template<typename T,typename ...A>
        constexpr bool stateless_nothrow_invocable_with_possible_temporary_storage() noexcept{
            if constexpr(requires_temporary_storage_when_called_with<T,A...>){
                return stateless_nothrow_invocable<T,temporary_storage_type_of_t<T,A...>,A...>;
            }else{
                return stateless_nothrow_invocable<T,A...>;
            }
        }
    }
    template<typename T,typename ...A> requires(stateless_functor_with_possible_temporary_storage<T,A...>)
    constexpr inline bool stateless_nothrow_invocable_with_possible_temporary_storage = detail::stateless_nothrow_invocable_with_possible_temporary_storage<T,A...>();
    namespace detail{
        template<typename T,typename ...A> requires(stateless_functor_with_possible_temporary_storage<T,A...>)
        struct stateless_invoke_with_possible_temporary_storage_result{
            using type = stateless_invoke_result<T,A...>;
        };
        template<typename T,typename ...A> requires(stateless_functor_with_possible_temporary_storage<T,A...> && requires_temporary_storage_when_called_with<T,A...>)
        struct stateless_invoke_with_possible_temporary_storage_result<T,A...>{
            using type = stateless_invoke_result<T,temporary_storage_type_of_t<T,A...>,A...>;
        };
    }
    template<typename T,typename ...A> requires(stateless_functor_with_possible_temporary_storage<T,A...>)
    using stateless_invoke_with_possible_temporary_storage_result = detail::stateless_invoke_with_possible_temporary_storage_result<T,A...>::type;
    template<typename T,typename ...A> requires(stateless_functor_with_possible_temporary_storage<T,A...>)
    constexpr decltype(auto) invoke_stateless_with_temporary_storage(coalesce_void_type_t<temporary_storage_type_of_t<T,A...>,std::monostate>&& strg,A&& ...a) noexcept(stateless_nothrow_invocable_with_possible_temporary_storage<T,A...>){
        if constexpr(requires_temporary_storage_when_called_with<T,A...>){
            return invoke_stateless<T>(std::move(strg),std::forward<A>(a)...);
        }else{
            static_cast<void>(strg);
            return invoke_stateless<T>(std::forward<A>(a)...);
        }
    }
    namespace detail{
        template<typename T>
        using store_if_nonreference = std::conditional_t<std::is_reference_v<T>,std::monostate,constexpr_uninitialized<T>>;
        template<typename ...T>
        struct tuple_or_stateless{
            using type = std::tuple<T...>;
        };
        template<std::same_as<std::monostate> ...T>
        struct tuple_or_stateless<T...>{
            using type = void;
        };
        template<typename ...T>
        using tuple_or_stateless_t = tuple_or_stateless<T...>::type;
    }
    template<typename T,typename U>
    struct atop{
        template<typename ...A>
        using temporary_storage_type = detail::tuple_or_stateless_t<
            coalesce_void_type_t<temporary_storage_type_of_t<T,stateless_invoke_with_possible_temporary_storage_result<U,A...>>,std::monostate>,
            coalesce_void_type_t<temporary_storage_type_of_t<U,A...>,std::monostate>,
            detail::store_if_nonreference<stateless_invoke_with_possible_temporary_storage_result<U,A...>>
        >;
        template<typename ...A> requires(
            stateless_functor_with_possible_temporary_storage<U,A...>
            && stateless_functor_with_possible_temporary_storage<T,stateless_invoke_with_possible_temporary_storage_result<U,A...>>
            && !std::same_as<temporary_storage_type<A...>,void>
        )
        constexpr static decltype(auto) operator()(temporary_storage_type<A...>&& strg,A&& ...a) noexcept(stateless_nothrow_invocable_with_possible_temporary_storage<U,A...> && stateless_nothrow_invocable_with_possible_temporary_storage<T,stateless_invoke_with_possible_temporary_storage_result<U,A...>>){
            if constexpr(std::is_reference_v<stateless_invoke_with_possible_temporary_storage_result<U,A...>>){
                return invoke_stateless_with_temporary_storage<T>(std::get<0uz>(std::move(strg)),invoke_stateless_with_temporary_storage<U>(std::get<1uz>(std::move(strg)),std::forward<A>(a)...));
            }else{
                stateless_invoke_with_possible_temporary_storage_result<U,A...>& intermediate = *::new(&std::get<2uz>(strg).member) stateless_invoke_with_possible_temporary_storage_result<U,A...>(invoke_stateless_with_temporary_storage<U>(std::get<1uz>(std::move(strg)),std::forward<A>(a)...));
                try{
                    return invoke_stateless_with_temporary_storage<T>(std::get<0uz>(std::move(strg)),std::move(intermediate));
                }catch(...){
                    std::destroy_at(&intermediate);
                    // GCC erroneously warns about a rethrow statement calling terminate even if the try-block is in fact noexcept
                    #pragma GCC diagnostic push
                    #pragma GCC diagnostic ignored "-Wterminate"
                    throw;
                    #pragma GCC diagnostic pop
                }
            }
        }
        template<typename ...A> requires(stateless_functor<U,A...> && stateless_functor<T,stateless_invoke_result<U,A...>> && std::same_as<temporary_storage_type<A...>,void>)
        constexpr static decltype(auto) operator()(A&& ...a) noexcept(stateless_nothrow_invocable<U,A...> && stateless_nothrow_invocable<T,stateless_invoke_result<U,A...>>){
            return invoke_stateless<T>(invoke_stateless<U>(std::forward<A>(a)...));
        }
    };
}
