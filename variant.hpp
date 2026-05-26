#pragma once
#include"polyfill/pack-indexing.hpp"
#include"exchange.hpp"
#include<type_traits>
#include<stdexcept>
#include<concepts>
#include<cassert>
#include<cstdint>
#include<utility>
#include<limits>
#include<meta>
namespace cppp{
    template<typename T>
    struct emplace_tag_t{};
    template<typename T>
    constexpr inline emplace_tag_t<T> emplace_tag{};
    namespace detail{
        template<typename T,typename ...P>
        constexpr inline std::size_t pack_find_i = [] consteval{
            using namespace std::literals;
            if constexpr(sizeof...(P) == 0) throw std::logic_error("pack_find_i: searching in empty pack"s);
            constexpr std::meta::info dt = dealias(^^T);
            std::size_t i = 0uz;
            template for(constexpr std::meta::info pv : {dealias(^^P)...}){
                if(dt == pv) return i;
                ++i;
            }
            std::string tl{(... + (display_string_of(^^P)+", "s))};
            tl.pop_back();
            tl.back() = '}';
            throw std::logic_error("pack_find_i: No type "s+display_string_of(^^T)+" in types {"s+tl);
        }();
    }
    template<typename ...Tv>
    class heap_variant{
        void* data;
        std::size_t num;
        void destroy() noexcept{
            if(data){
                dispatch([]<typename T>(T& obj) static{
                    delete &obj;
                });
                data = nullptr;
            }
        }
        heap_variant clone() const{
            return dispatch([]<typename T>(T& obj) static -> heap_variant {
                return {obj};
            });
        }
        public:
            template<typename T>
            constexpr static std::size_t index_of = detail::pack_find_i<T,Tv...>;
            constexpr static std::size_t none{std::numeric_limits<std::size_t>::max()};
            constexpr heap_variant() noexcept : data(nullptr), num(0uz){}
            constexpr explicit heap_variant(std::size_t num,void* data) noexcept : data(data), num(num){}
            template<typename T> requires(... || std::same_as<std::remove_cvref_t<T>,Tv>)
            heap_variant(T&& inst) : data(new std::remove_cvref_t<T>(std::forward<T>(inst))), num(index_of<std::remove_cvref_t<T>>){}
            template<typename T,typename ...A>
            heap_variant(emplace_tag_t<T>,A&& ...argv) : data(new T(std::forward<A>(argv)...)), num(index_of<std::remove_cvref_t<T>>){}
            heap_variant(const heap_variant& other) : heap_variant(other.clone()){}
            constexpr heap_variant(heap_variant&& other) noexcept : data(std::exchange(other.data,nullptr)), num(other.num){}
            heap_variant& operator=(const heap_variant& other){
                *this = other.clone();
            }
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
                std::size_t n = num;
                template for(constexpr std::meta::info alt : {^^Tv...}){
                    if(!(n--)){
                        return std::forward<Fn>(fn)(*static_cast<const [:alt:]*>(data));
                    }
                }
                std::unreachable();
            }
            template<typename Fn>
            constexpr decltype(auto) dispatch(Fn&& fn) const{
                std::size_t n = num;
                template for(constexpr std::meta::info alt : {^^Tv...}){
                    if(!(n--)){
                        return std::forward<Fn>(fn)(*static_cast<const [:alt:]*>(data));
                    }
                }
                std::unreachable();
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
            template<typename T>
            constexpr bool has_unchecked() const noexcept{
                return num == index_of<T>;
            }
            template<typename T>
            constexpr bool has() const noexcept{
                return !empty() && has_unchecked<T>();
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
