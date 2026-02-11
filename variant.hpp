#pragma once
#include"polyfill/pack-indexing.hpp"
#include"exchange.hpp"
#include<type_traits>
#include<cassert>
#include<cstdint>
#include<utility>
#include<limits>
namespace cppp{
    template<typename T>
    struct emplace_tag_t{};
    template<typename T>
    constexpr inline emplace_tag_t<T> emplace_tag{};
    namespace detail{
        template<typename T,typename ...Types>
        struct pack_find{};
        template<typename T,typename ...Types>
        struct pack_find<T,T,Types...>{
            constexpr static inline std::size_t index = 0uz;
        };
        template<typename T,typename U1,typename ...Types> requires(!std::same_as<U1,T>)
        struct pack_find<T,U1,T,Types...>{
            constexpr static inline std::size_t index = 1uz;
        };
        template<typename T,typename U1,typename U2,typename ...Types> requires(!std::is_same_v<U1,T>&&!std::is_same_v<U2,T>)
        struct pack_find<T,U1,U2,T,Types...>{
            constexpr static inline std::size_t index = 2uz;
        };
        template<typename T,typename U1,typename U2,typename U3,typename ...Types> requires(!std::is_same_v<U1,T>&&!std::is_same_v<U2,T>&&!std::is_same_v<U3,T>)
        struct pack_find<T,U1,U2,U3,Types...>{
            constexpr static inline std::size_t index = pack_find<T,Types...>::index+3uz;
        };
        template<typename T,typename ...Types>
        constexpr inline std::size_t pack_find_i = pack_find<T,Types...>::index;
        struct destroy{
            template<typename T>
            void operator()(T& obj) const noexcept{
                delete &obj;
            }
        };
    }
    template<typename ...Tv>
    class heap_variant{
        void* data;
        std::size_t num;
        void destroy() noexcept{
            if(data){
                dispatch(detail::destroy());
                data = nullptr;
            }
        }
        template<typename Fn,std::size_t i> requires(i<sizeof...(Tv))
        constexpr decltype(auto) _dispatch(Fn& fn){
            if(i==num){
                return std::forward<Fn>(fn)(*static_cast<compat::index_pack<i,Tv...>*>(data));
            }else if constexpr(i+1uz==sizeof...(Tv)){
                std::unreachable();
            }else{
                return _dispatch<Fn,i+1uz>(std::forward<Fn>(fn));
            }
        }
        template<typename Fn,std::size_t i> requires(i<sizeof...(Tv))
        constexpr decltype(auto) _dispatch(Fn&& fn) const{
            if(i==num){
                return std::forward<Fn>(fn)(*static_cast<const compat::index_pack<i,Tv...>*>(data));
            }else if constexpr(i+1uz==sizeof...(Tv)){
                std::unreachable();
            }else{
                return _dispatch<Fn,i+1uz>(std::forward<Fn>(fn));
            }
        }
        public:
            template<typename T>
            constexpr static std::size_t index_of = detail::pack_find_i<T,Tv...>;
            constexpr static std::size_t none{std::numeric_limits<std::size_t>::max()};
            constexpr heap_variant() noexcept : data(nullptr), num(0uz){}
            constexpr explicit heap_variant(std::size_t num,void* data) noexcept : data(data), num(num){}
            template<typename T>
            heap_variant(T&& inst) : data(new std::remove_cvref_t<T>(std::forward<T>(inst))), num(index_of<std::remove_cvref_t<T>>){}
            template<typename T,typename ...A>
            heap_variant(emplace_tag_t<T>,A&& ...argv) : data(new T(std::forward<A>(argv)...)), num(index_of<std::remove_cvref_t<T>>){}
            heap_variant(const heap_variant&) = delete;
            constexpr heap_variant(heap_variant&& other) noexcept : data(std::exchange(other.data,nullptr)), num(other.num){}
            heap_variant& operator=(const heap_variant&) = delete;
            constexpr heap_variant& operator=(heap_variant&& other) noexcept{
                reset(other.num,std::exchange(other.data,nullptr));
                return *this;
            }
            constexpr void reset() noexcept{
                data = nullptr;
            }
            constexpr void reset(std::size_t n,void* d) noexcept{
                destroy();
                num = n;
                data = d;
            }
            template<typename Fn>
            constexpr decltype(auto) dispatch(Fn&& fn){
                assert(*this);
                return _dispatch<Fn,0uz>(std::forward<Fn>(fn));
            }
            template<typename Fn>
            constexpr decltype(auto) dispatch(Fn&& fn) const{
                assert(*this);
                return _dispatch<Fn,0uz>(std::forward<Fn>(fn));
            }
            template<typename T,typename ...A>
            constexpr void emplace(A&& ...argv){
                reset(index_of<T>,new T(std::forward<A>(argv)...));
            }
            template<std::size_t i,typename ...A>
            constexpr void emplace(A&& ...argv){
                using T = compat::index_pack<i,Tv...>;
                reset(i,new T(std::forward<A>(argv)...));
            }
            template<typename T>
            constexpr T& get() noexcept{
                return *static_cast<T*>(data);
            }
            template<typename T>
            constexpr const T& get() const noexcept{
                return *static_cast<const T*>(data);
            }
            template<std::size_t i>
            constexpr compat::index_pack<i,Tv...>& get() noexcept{
                return get<compat::index_pack<i,Tv...>>();
            }
            template<std::size_t i>
            constexpr const compat::index_pack<i,Tv...>& get() const noexcept{
                return get<compat::index_pack<i,Tv...>>();
            }
            constexpr std::size_t index() const noexcept{
                return num;
            }
            constexpr std::size_t tell() const noexcept{
                return empty()?none:num;
            }
            constexpr bool empty() const noexcept{
                return data == nullptr;
            }
            constexpr explicit operator bool() const noexcept{
                return data;
            }
            ~heap_variant(){
                destroy();
            }
    };
}
