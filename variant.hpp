#pragma once
#include<type_traits>
#include<algorithm>
#include<ranges>
#include<vector>
#include<string>
#include<cmath>
#ifndef __INTELLISENSE__
#include<meta>
#endif
#include<new>
#include"concepts.hpp"
#include"launder.hpp"
#include"assert.hpp"
#include"trap.hpp"
#include"eto.hpp"
#include"rtl.hpp"
#ifdef __INTELLISENSE__
namespace cppp{
    template<enumeration auto v>
    struct in_place_etor_t{};
    template<enumeration auto v>
    constexpr inline in_place_etor_t<v> in_place_etor;
    template<enumeration E>
    class variant{
        public:
            template<E v>
            using lookup = __error_type;
            variant() noexcept;
            template<E val,typename ...A>
            variant(in_place_etor_t<val>,A&& ...a);
            variant(const variant& other);
            variant(variant&& other);
            variant& operator=(const variant& other);
            variant& operator=(variant&& other);
            explicit operator bool();
            E tag() const noexcept;
            template<E val,typename ...A>
            __error_type emplace(A&& ...a);
            template<E val>
            const __error_type& get() const noexcept;
            template<E val>
            __error_type& get() noexcept;
            bool has(E val) const noexcept;
            template<typename Fn>
            __error_type visit(Fn&& fn);
            template<typename Fn>
            __error_type visit(Fn&& fn) const;
            ~variant();
    };
    template<enumeration E>
    class heap_variant{
        public:
            template<E v>
            using lookup = __error_type;
            constexpr heap_variant() noexcept;
            template<E val,typename ...A>
            constexpr heap_variant(in_place_etor_t<val>,A&& ...a);
            constexpr heap_variant(const heap_variant& other);
            constexpr heap_variant(heap_variant&& other);
            constexpr heap_variant& operator=(const heap_variant& other);
            constexpr heap_variant& operator=(heap_variant&& other);
            constexpr explicit operator bool();
            constexpr E tag() const noexcept;
            template<E val,typename ...A>
            constexpr __error_type emplace(A&& ...a);
            template<E val>
            constexpr const __error_type& get() const noexcept;
            template<E val>
            constexpr __error_type& get() noexcept;
            constexpr bool has(E val) const noexcept;
            template<typename Fn>
            constexpr __error_type visit(Fn&& fn);
            template<typename Fn>
            constexpr __error_type visit(Fn&& fn) const;
            constexpr ~heap_variant();
    };
}
#else
namespace cppp{
    namespace detail{
        struct etor_annot{
            std::meta::info type;
        };
        consteval std::meta::info etor_type(std::meta::info etor){
            using namespace std::literals;
            if(std::vector<std::meta::info> etor_annots{annotations_of_with_type(etor,^^etor_annot)};!etor_annots.empty()){
                if(etor_annots.size() > 1uz) throw std::meta::exception(u8"cppp::variant: enumerator has more than one annotation"sv,etor);
                return extract<etor_annot>(etor_annots.front()).type;
            }else{
                std::vector<std::meta::info> annots{annotations_of(etor)};
                if(annots.empty()) return ^^void;
                if(annots.size() == 1uz && type_of(annots.front()) == dealias(^^std::meta::info) && is_type(extract<std::meta::info>(annots.front()))){
                    // GCC, you should know better than to complain that NRVO is not done on a std::meta::info instance...
                    // I know we can just copy it manually, but let's do the Right Thing and follow https://marralesfios.github.io/blog/nrvo
                    return [&annots,etor]consteval{
                        std::meta::info type = extract<std::meta::info>(annots.front());
                        if(is_lvalue_reference_type(type) || is_rvalue_reference_type(type)){
                            throw std::meta::exception(u8"cppp::variant: enumerator type must not be a reference type"sv,etor);
                        }
                        if(is_function_type(type)){
                            throw std::meta::exception(u8"cppp::variant: enumerator type must not be a function"sv,etor);
                        }
                        return type;
                    }();
                }else throw std::meta::exception(u8"cppp::variant: enumerator does not have an unambiguous type annotation"sv,etor);
            }
        }
        template<std::size_t n>
        struct store_value{
            constexpr static std::size_t value = n;
        };
        template<bool b>
        struct store_flag{
            constexpr static bool value = b;
        };
        template<typename E>
        struct etor_info{
            E v;
            std::meta::info t;
        };
        template<typename E,etor_info<E> ...etor_infos>
        class enum_info_base{
            public:
                constexpr static std::array<etor_info<E>,sizeof...(etor_infos)> infos{etor_infos...};
            private:
                template<E v>
                struct _lookup;
                consteval{
                    using namespace std::literals;
                    for(std::size_t i=0uz;i<infos.size();++i){
                        define_aggregate(substitute(^^_lookup,{std::meta::reflect_constant(infos[i].v)}),{data_member_spec(substitute(^^store_value,{std::meta::reflect_constant(i)}),{.name=u8"_tag"sv})});
                    }
                }
                template<E v>
                constexpr static std::size_t member_lookup = decltype(_lookup<v>::_tag)::value;
            public:
                template<E v>
                using lookup = [:infos[member_lookup<v>].t:];
                template<E v>
                constexpr static std::meta::info lookup_member = infos[member_lookup<v>].mem;
                consteval static bool is_nothrow_copy_constructible() noexcept{
                    for(const detail::etor_info<E>& ei : infos){
                        if(!is_nothrow_copy_constructible_type(ei.t)) return false;
                    }
                    return true;
                }
                consteval static bool is_nothrow_move_constructible() noexcept{
                    for(const detail::etor_info<E>& ei : infos){
                        if(!is_nothrow_move_constructible_type(ei.t)) return false;
                    }
                    return true;
                }
                constexpr static bool is_optional = is_void_type(infos[0uz].t);
                consteval static bool is_nothrow_copy_assignable() noexcept{
                    if(!is_optional) return true;
                    if(!is_nothrow_copy_constructible()) return false;
                    for(const detail::etor_info<E>& ei : infos){
                        if(!is_nothrow_copy_assignable_type(ei.t)) return false;
                    }
                    return true;
                }
                consteval static bool is_nothrow_move_assignable() noexcept{
                    if(!is_optional) return true;
                    if(!is_nothrow_move_constructible()) return false;
                    for(const detail::etor_info<E>& ei : infos){
                        if(!is_nothrow_move_assignable_type(ei.t)) return false;
                    }
                    return true;
                }
        };
        template<typename E,std::size_t s,std::size_t a,etor_info<E> ...etor_infos>
        class inplace_enum_info_base : public enum_info_base<E,etor_infos...>{
            public:
                constexpr static std::size_t size = s;
                constexpr static std::size_t alignment = a;
        };
        template<enumeration E>
        using enum_info = [:[]consteval static{
            std::vector<std::meta::info> etor_infos{^^E};
            for(std::meta::info etor : enumerators_of(^^E)){
                std::meta::info t = etor_type(etor);
                etor_infos.emplace_back(std::meta::reflect_constant(etor_info<E>{.v=extract<E>(etor),.t=t}));
            }
            return substitute(^^enum_info_base,etor_infos);
        }():];
        template<enumeration E>
        using inplace_enum_info = [:[]consteval static{
            std::vector<std::meta::info> etor_infos;
            std::size_t size = 0uz;
            std::size_t alignment = 1uz;
            for(std::meta::info etor : enumerators_of(^^E)){
                std::meta::info t = etor_type(etor);
                etor_infos.emplace_back(std::meta::reflect_constant(etor_info<E>{.v=extract<E>(etor),.t=t}));
                if(!is_void_type(t)){
                    size = std::max(size,size_of(t));
                    alignment = std::max(alignment,alignment_of(t));
                }
            }
            return substitute(^^inplace_enum_info_base,std::views::concat(rtl<std::initializer_list<std::meta::info>>({^^E,std::meta::reflect_constant(size),std::meta::reflect_constant(alignment)}),etor_infos));
        }():];
        struct inplace_destroy{
            constexpr static void operator()() noexcept{}
            template<typename T>
            constexpr static void operator()(T& v) noexcept(std::is_nothrow_destructible_v<T>){
                v.~T();
            }
        };
    }
    template<typename T>
    constexpr inline detail::etor_annot etor{.type = ^^T};
    template<enumeration auto v>
    struct in_place_etor_t{};
    template<enumeration auto v>
    constexpr inline in_place_etor_t<v> in_place_etor;
    template<enumeration E>
    class variant{
        #if __cpp_consteval >= 202406L
        #warning P4101 might be merged! Check to see if access_constexpr_etor is still necessary.
        #endif
        // At least before P4101, etor_info<E> being a consteval-only type makes accessing infos[...].v promote the entire containing function to consteval. This is unacceptable.
        template<E v>
        constexpr static E access_constexpr_etor = v;
        using info_t = detail::inplace_enum_info<E>;
        [[no_unique_address]] potentially_empty_array<std::byte,info_t::size,info_t::alignment> data;
        E _tag;
        template<typename T>
        T& _get() noexcept{
            return read_object<T>(data.data());
        }
        template<typename T>
        const T& _get() const noexcept{
            return read_object<T>(data.data());
        }
        void _destroy() noexcept{
            visit(detail::inplace_destroy());
        }
        template<typename T,typename ...A>
        void emplace_construct(A&& ...a) noexcept(std::is_nothrow_constructible_v<T,A...>){
            new(data.data()) T(std::forward<A>(a)...);
        }
        template<typename T,typename ...A>
        void _emplace(A&& ...a) noexcept(!info_t::is_optional || std::is_nothrow_constructible_v<T,A...>){
            _destroy();
            if constexpr(info_t::is_optional && !std::is_nothrow_constructible_v<T,A...>){
                try{
                    emplace_construct<T>(std::forward<A>(a)...);
                }catch(...){
                    _tag = access_constexpr_etor<info_t::infos[0uz].v>;
                    throw;
                }
            }else{
                emplace_construct<T>(std::forward<A>(a)...);
            }
        }
        public:
            template<E v>
            using lookup = info_t::template lookup<v>;
            variant() noexcept requires(info_t::is_optional) : _tag(access_constexpr_etor<info_t::infos[0uz].v>){}
            template<E val,typename ...A> requires(!std::is_void_v<lookup<val>>)
            variant(in_place_etor_t<val>,A&& ...a) noexcept(noexcept(new(data.data()) lookup<val>(std::forward<A>(a)...))) : _tag(val){
                emplace_construct<lookup<val>>(std::forward<A>(a)...);
            }
            template<E val> requires(std::is_void_v<lookup<val>>)
            variant(in_place_etor_t<val>) noexcept : _tag(val){}
            variant(const variant& other) noexcept(info_t::is_nothrow_copy_constructible()) : _tag(other._tag){
                template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                    if constexpr(!is_void_type(ei.t)){
                        if(ei.v == _tag){
                            emplace_construct<typename[:ei.t:]>(other._get<typename[:ei.t:]>());
                        }
                    }
                }
            }
            variant(variant&& other) noexcept(info_t::is_nothrow_move_constructible()) : _tag(other._tag){
                template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                    if constexpr(!is_void_type(ei.t)){
                        if(ei.v == _tag){
                            emplace_construct<typename[:ei.t:]>(std::move(other._get<typename[:ei.t:]>()));
                        }
                    }
                }
            }
            variant& operator=(const variant& other) noexcept(info_t::is_nothrow_copy_assignable()){
                if(_tag == other._tag){
                    template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                        if constexpr(!is_void_type(ei.t)){
                            if(ei.v == _tag){
                                _get<typename[:ei.t:]>() = other._get<typename[:ei.t:]>();
                            }
                        }
                    }
                }else{
                    template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                        if constexpr(!is_void_type(ei.t)){
                            if(ei.v == other._tag){
                                _emplace<typename[:ei.t:]>(other._get<typename[:ei.t:]>());
                            }
                        }
                    }
                    _tag = other._tag;
                }
                return *this;
            }
            variant& operator=(variant&& other) noexcept(info_t::is_nothrow_move_assignable()){
                if(_tag == other._tag){
                    template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                        if constexpr(!is_void_type(ei.t)){
                            if(ei.v == _tag){
                                _get<typename[:ei.t:]>() = std::move(other._get<typename[:ei.t:]>());
                            }
                        }
                    }
                }else{
                    template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                        if constexpr(!is_void_type(ei.t)){
                            if(ei.v == other._tag){
                                _emplace<typename[:ei.t:]>(std::move(other._get<typename[:ei.t:]>()));
                            }
                        }
                    }
                    _tag = other._tag;
                }
                return *this;
            }
            explicit operator bool() requires(info_t::is_optional){
                return _tag != access_constexpr_etor<info_t::infos[0uz].v>;
            }
            E tag() const noexcept{
                return _tag;
            }
            template<E val,typename ...A>
            lookup<val>& emplace(A&& ...a) noexcept(noexcept(new(data.data()) lookup<val>(std::forward<A>(a)...))){
                _destroy();
                _tag = val;
                return *new(data.data()) lookup<val>(std::forward<A>(a)...);
            }
            template<E val>
            void emplace() noexcept requires(std::is_void_v<lookup<val>>){
                _destroy();
                _tag = val;
            }
            template<E val>
            const lookup<val>& get() const noexcept{
                CPPP_ASSERT(_tag == val);
                return _get<lookup<val>>();
            }
            template<E val>
            lookup<val>& get() noexcept{
                CPPP_ASSERT(_tag == val);
                return _get<lookup<val>>();
            }
            template<typename Fn>
            decltype(auto) visit(Fn&& fn){
                template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                    if(ei.v == _tag){
                        if constexpr(is_void_type(ei.t)){
                            return std::forward<Fn>(fn)();
                        }else{
                            return std::forward<Fn>(fn)(_get<typename[:ei.t:]>());
                        }
                    }
                }
                unreachable();
            }
            template<typename Fn>
            decltype(auto) visit(Fn&& fn) const{
                template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                    if(ei.v == _tag){
                        if constexpr(is_void_type(ei.t)){
                            return std::forward<Fn>(fn)();
                        }else{
                            return std::forward<Fn>(fn)(_get<typename[:ei.t:]>());
                        }
                    }
                }
                unreachable();
            }
            ~variant(){
                _destroy();
            }
    };
    namespace detail{
        template<typename T>
        concept eligible_for_soo = sizeof(T) <= sizeof(void*) && alignof(T) <= alignof(void*);
    }
    template<enumeration E>
    class heap_variant{
        #if __cpp_consteval >= 202406L
        #warning P4101 might be merged! Check to see if access_constexpr_etor is still necessary.
        #endif
        // At least before P4101, etor_info<E> being a consteval-only type makes accessing infos[...].v promote the entire containing function to consteval. This is unacceptable.
        template<E v>
        constexpr static E access_constexpr_etor = v;
        using info_t = detail::enum_info<E>;
        alignas(void*) std::byte data[sizeof(void*)];
        E _tag;
        template<typename T>
        constexpr T& _get() noexcept{
            if constexpr(detail::eligible_for_soo<T>){
                return read_object<T>(data);
            }else{
                return *static_cast<T*>(read_object<void*>(data));
            }
        }
        template<typename T>
        constexpr const T& _get() const noexcept{
            if constexpr(detail::eligible_for_soo<T>){
                return read_object<T>(data);
            }else{
                return *static_cast<const T*>(read_object<void*>(data));
            }
        }
        constexpr void _destroy() noexcept{
            template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                if(ei.v == _tag){
                    if constexpr(!is_void_type(ei.t)){
                        typename[:ei.t:]* ptr = &_get<typename[:ei.t:]>();
                        if constexpr(detail::eligible_for_soo<typename[:ei.t:]>){
                            std::destroy_at(ptr);
                        }else{
                            delete ptr;
                            std::destroy_at(&read_object<void*>(data));
                        }
                    }
                }
            }
        }
        template<typename T,typename ...A>
        constexpr void emplace_construct(A&& ...a) noexcept(detail::eligible_for_soo<T> && std::is_nothrow_constructible_v<T,A...>){
            if constexpr(detail::eligible_for_soo<T>){
                new(data) T(std::forward<A>(a)...);
            }else{
                using voidp = void*;
                new(data) voidp(new T(std::forward<A>(a)...));
            }
        }
        template<typename T,typename ...A>
        constexpr void _emplace(A&& ...a) noexcept(!info_t::is_optional || (detail::eligible_for_soo<T> && std::is_nothrow_constructible_v<T,A...>)){
            if constexpr(detail::eligible_for_soo<T>){
                _destroy();
                if constexpr(info_t::is_optional && !std::is_nothrow_constructible_v<T,A...>){
                    try{
                        emplace_construct<T>(std::forward<A>(a)...);
                    }catch(...){
                        _tag = access_constexpr_etor<info_t::infos[0uz].v>;
                        throw;
                    }
                }else{
                    emplace_construct<T>(std::forward<A>(a)...);
                }
            }else{
                T* alloc = new T(std::forward<A>(a)...);
                _destroy();
                using voidp = void*;
                new(data) voidp(alloc);
            }
        }
        public:
            template<E v>
            using lookup = info_t::template lookup<v>;
            constexpr heap_variant() noexcept requires(info_t::is_optional) : data{}, _tag(access_constexpr_etor<info_t::infos[0uz].v>){}
            template<E val,typename ...A> requires(!std::is_void_v<lookup<val>>)
            constexpr heap_variant(in_place_etor_t<val>,A&& ...a) : data{}, _tag(val){
                emplace_construct<lookup<val>>(std::forward<A>(a)...);
            }
            template<E val> requires(std::is_void_v<lookup<val>>)
            constexpr heap_variant(in_place_etor_t<val>) noexcept : data{}, _tag(val){}
            constexpr heap_variant(const heap_variant& other) noexcept(info_t::is_nothrow_copy_constructible()) : data{}, _tag(other._tag){
                template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                    if constexpr(!is_void_type(ei.t)){
                        if(ei.v == _tag){
                            emplace_construct<typename[:ei.t:]>(other._get<typename[:ei.t:]>());
                        }
                    }
                }
            }
            constexpr heap_variant(heap_variant&& other) noexcept(info_t::is_nothrow_move_constructible()) : data{}, _tag(other._tag){
                template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                    if constexpr(!is_void_type(ei.t)){
                        if(ei.v == _tag){
                            emplace_construct<typename[:ei.t:]>(std::move(other._get<typename[:ei.t:]>()));
                        }
                    }
                }
            }
            constexpr heap_variant& operator=(const heap_variant& other) noexcept(info_t::is_nothrow_copy_assignable()){
                if(_tag == other._tag){
                    template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                        if constexpr(!is_void_type(ei.t)){
                            if(ei.v == _tag){
                                _get<typename[:ei.t:]>() = other._get<typename[:ei.t:]>();
                            }
                        }
                    }
                }else{
                    template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                        if constexpr(!is_void_type(ei.t)){
                            if(ei.v == other._tag){
                                _emplace<typename[:ei.t:]>(other._get<typename[:ei.t:]>());
                            }
                        }
                    }
                    _tag = other._tag;
                }
                return *this;
            }
            constexpr heap_variant& operator=(heap_variant&& other) noexcept(info_t::is_nothrow_move_constructible()){
                if(_tag == other._tag){
                    template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                        if(ei.v == _tag){
                            if constexpr(!is_void_type(ei.t)){
                                _get<typename[:ei.t:]>() = std::move(other._get<typename[:ei.t:]>());
                            }
                        }
                    }
                }else{
                    template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                        if constexpr(!is_void_type(ei.t)){
                            if(ei.v == other._tag){
                                _emplace<typename[:ei.t:]>(std::move(other._get<typename[:ei.t:]>()));
                            }
                        }
                    }
                    _tag = other._tag;
                }
                return *this;
            }
            constexpr explicit operator bool() requires(info_t::is_optional){
                return _tag != access_constexpr_etor<info_t::infos[0uz].v>;
            }
            constexpr E tag() const noexcept{
                return _tag;
            }
            template<E val,typename ...A>
            constexpr lookup<val>& emplace(A&& ...a) noexcept(!info_t::is_optional || (detail::eligible_for_soo<lookup<val>> && std::is_nothrow_constructible_v<lookup<val>,A...>)){
                _emplace<lookup<val>>(std::forward<A>(a)...);
                _tag = val;
                return _get<lookup<val>>();
            }
            template<E val>
            constexpr void emplace() noexcept requires(std::is_void_v<lookup<val>>){
                _destroy();
                _tag = val;
            }
            template<E val>
            constexpr const lookup<val>& get() const noexcept{
                if consteval{
                    cppp::consteval_assert(_tag == val);
                }else{
                    CPPP_ASSERT(_tag == val);
                }
                return _get<lookup<val>>();
            }
            template<E val>
            constexpr lookup<val>& get() noexcept{
                if consteval{
                    cppp::consteval_assert(_tag == val);
                }else{
                    CPPP_ASSERT(_tag == val);
                }
                return _get<lookup<val>>();
            }
            template<typename Fn>
            constexpr decltype(auto) visit(Fn&& fn){
                template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                    if(ei.v == _tag){
                        if constexpr(is_void_type(ei.t)){
                            return std::forward<Fn>(fn)();
                        }else{
                            return std::forward<Fn>(fn)(_get<typename[:ei.t:]>());
                        }
                    }
                }
                unreachable();
            }
            template<typename Fn>
            constexpr decltype(auto) visit(Fn&& fn) const{
                template for(constexpr const detail::etor_info<E>& ei : info_t::infos){
                    if(ei.v == _tag){
                        if constexpr(is_void_type(ei.t)){
                            return std::forward<Fn>(fn)();
                        }else{
                            return std::forward<Fn>(fn)(_get<typename[:ei.t:]>());
                        }
                    }
                }
                unreachable();
            }
            constexpr ~heap_variant(){
                _destroy();
            }
    };
}
#endif
