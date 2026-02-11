#pragma once
#include"assert.hpp"
#include<type_traits>
#include<concepts>
#include<iterator>
#include<cstdint>
#include<utility>
#include<cassert>
#include<span>
namespace cppp{
    namespace detail{
        template<typename T,typename B>
        concept derived_from = std::same_as<B,void> || std::derived_from<T,B>;
        template<typename T,typename U>
        concept more_const_or_equal_than = (std::is_const_v<T> || !std::is_const_v<U>);
    }
    template<typename Base>
    class erased_ptr{
        using data_type = std::conditional_t<std::is_const_v<Base>,const std::byte,std::byte>;
        data_type* m;
        std::uint32_t stride;
        template<typename Other>
        friend class erased_ptr;
        constexpr erased_ptr(data_type* m,std::uint32_t stride) noexcept : m(m), stride(stride){}
        public:
            // Make usable as iterators.
            using value_type = Base;
            using reference = std::add_lvalue_reference_t<Base>;
            using pointer = Base*;
            using difference_type = std::ptrdiff_t;

            constexpr erased_ptr() noexcept : m(nullptr), stride(0){}
            constexpr erased_ptr(std::nullptr_t) noexcept : erased_ptr(){}
            template<detail::derived_from<Base> Derived> requires(detail::more_const_or_equal_than<Base,Derived>)
            constexpr erased_ptr(Derived* m) noexcept : erased_ptr(reinterpret_cast<data_type*>(static_cast<Base*>(m)),sizeof(Derived)){}
            template<detail::derived_from<Base> Derived> requires(detail::more_const_or_equal_than<Base,Derived>)
            constexpr erased_ptr(erased_ptr<Derived> other) noexcept : erased_ptr(reinterpret_cast<data_type*>(static_cast<Base*>(other.get())),other.stride){}
            constexpr Base* get() const noexcept{
                return reinterpret_cast<Base*>(m);
            }
            constexpr Base* operator->() const noexcept{
                return get();
            }
            constexpr reference operator*() const noexcept requires(!std::is_void_v<Base>){
                return *get();
            }
            constexpr reference operator[](std::ptrdiff_t index) const noexcept requires(!std::is_void_v<Base>){
                return *reinterpret_cast<Base*>(m+index*static_cast<std::ptrdiff_t>(stride));
            }
            constexpr erased_ptr<Base>& operator+=(std::ptrdiff_t delta) noexcept{
                m += delta*static_cast<std::ptrdiff_t>(stride);
                return *this;
            }
            constexpr erased_ptr<Base>& operator-=(std::ptrdiff_t delta) noexcept{
                m -= delta*static_cast<std::ptrdiff_t>(stride);
                return *this;
            }
            constexpr erased_ptr<Base> operator-(this erased_ptr<Base> self,std::ptrdiff_t delta) noexcept{
                return self -= delta;
            }
            constexpr erased_ptr<Base>& operator++() noexcept{
                m += stride;
                return *this;
            }
            constexpr erased_ptr<Base> operator++(int) noexcept{
                erased_ptr<Base> dup{*this};
                m += stride;
                return dup;
            }
            constexpr erased_ptr<Base>& operator--() noexcept{
                m -= stride;
                return *this;
            }
            constexpr erased_ptr<Base> operator--(int) noexcept{
                erased_ptr<Base> dup{*this};
                m -= stride;
                return dup;
            }
            constexpr std::ptrdiff_t operator-(erased_ptr<Base> other) const noexcept{
                CPPP_ASSERT(stride == other.stride);
                return (m-other.m)/static_cast<std::ptrdiff_t>(stride);
            }
            constexpr std::strong_ordering operator<=>(erased_ptr<Base> other) const noexcept{
                return m <=> other.m;
            }
            template<detail::derived_from<Base> Derived> requires(detail::more_const_or_equal_than<Derived,Base>)
            constexpr erased_ptr<Derived> downcast() const noexcept{
                CPPP_ASSERT(sizeof(Derived) <= stride);
                return {reinterpret_cast<data_type*>(static_cast<Derived*>(get())),stride};
            }
            constexpr bool operator==(erased_ptr<Base> other) const noexcept{
                return m == other.m && stride == other.stride;
            }
    };
    template<typename Base>
    constexpr erased_ptr<Base> operator+(erased_ptr<Base> lhs,std::ptrdiff_t delta) noexcept{
        return lhs += delta;
    }
    template<typename Base>
    constexpr erased_ptr<Base> operator+(std::ptrdiff_t delta,erased_ptr<Base> rhs) noexcept{
        return rhs += delta;
    }
    template<typename T,typename SizeT>
    class basic_erased_span : private erased_ptr<T>{
        SizeT _size;
        template<typename U,typename SizeU>
        friend class basic_erased_span;
        constexpr basic_erased_span(erased_ptr<T> parent,SizeT size) noexcept : erased_ptr<T>(parent), _size(size){}
        public:
            using size_type = SizeT;
            template<detail::derived_from<T> Derived,std::size_t Extent> requires(detail::more_const_or_equal_than<T,Derived>)
            constexpr basic_erased_span(std::span<Derived,Extent> m) noexcept : erased_ptr<T>(m.data()), _size(static_cast<SizeT>(m.size())){
                CPPP_ASSERT(m.size() <= std::numeric_limits<SizeT>::max());
            }
            template<detail::derived_from<T> Derived> requires(detail::more_const_or_equal_than<T,Derived>)
            constexpr basic_erased_span(basic_erased_span<Derived,SizeT> other) noexcept : erased_ptr<T>(other.begin()), _size(size){}

            template<detail::derived_from<T> Derived> requires(detail::more_const_or_equal_than<Derived,T>)
            constexpr basic_erased_span<Derived,SizeT> downcast() const noexcept{
                return {erased_ptr<T>::template downcast<Derived>(),_size};
            }
            using iterator = erased_ptr<T>;
            constexpr SizeT size() const noexcept{
                return _size;
            }
            constexpr bool empty() const noexcept{
                return !_size;
            }
            constexpr iterator begin() const noexcept{
                return *this;
            }
            constexpr iterator end() const noexcept{
                return begin()+static_cast<std::ptrdiff_t>(_size);
            }
            constexpr erased_ptr<T>::reference front() const noexcept requires(!std::is_void_v<T>){
                return *begin();
            }
            constexpr erased_ptr<T>::reference back() const noexcept requires(!std::is_void_v<T>){
                return (*this)[_size-1];
            }
            constexpr erased_ptr<T>::reference operator[](std::size_t i) const noexcept requires(!std::is_void_v<T>){
                return (*static_cast<const erased_ptr<T>*>(this))[static_cast<std::ptrdiff_t>(i)];
            }
    };
    template<typename T>
    using short_erased_span = basic_erased_span<T,std::uint32_t>;
    template<typename T>
    using erased_span = basic_erased_span<T,std::size_t>;
    #ifndef __INTELLISENSE__
    static_assert(sizeof(short_erased_span<void>) == 16,"ABI break: tail padding of erased_ptr not reused for short_erased_span.");
    #endif
}
