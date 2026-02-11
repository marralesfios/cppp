#pragma once
#include<initializer_list>
#include<algorithm>
#include<cstdint>
#include<cmath>
#include<span>
#include<cppp/zeroing-field.hpp>
#include"binary.hpp"
namespace cppp{
    class bytes{
        zeroing_field<std::byte*> _m;
        zeroing_field<std::size_t> _l;
        zeroing_field<std::size_t> _c;
        void _reallocate(){
            if(std::size_t cpc=*_c){
                std::byte* nbuf = new std::byte[cpc];
                std::copy(*_m,*_m+*_l,nbuf);
                delete[] std::exchange(*_m,nbuf);
            }else{
                delete[] std::exchange(*_m,nullptr);
            }
        }
        public:
            constexpr bytes() noexcept = default;
            constexpr bytes(bytes&&) noexcept = default;
            constexpr bytes& operator=(bytes&&) noexcept = default;
            bytes(std::initializer_list<std::byte> b){
                std::copy_n(b.begin(),b.size(),append(b.size()));
            }
            constexpr bool empty() const noexcept{
                return *_l == 0uz;
            }
            void append(frozenbuffer b){
                std::copy_n(b.data(),b.size(),append(b.size()));
            }
            std::byte* append(std::size_t n){
                std::size_t old_size = *_l;
                resize(*_l+n);
                return *_m+old_size;
            }
            template<std::size_t n>
            std::span<std::byte,n> append(){
                return std::span<std::byte,n>(append(n),n);
            }
            void append(std::byte v){
                *append(1uz) = v;
            }
            template<typename I>
            void appendl(I v){
                write<I>(append(sizeof(I)),v);
            }
            void reserve(std::size_t ns){
                while(ns>*_c){
                    *_c += std::max(*_c,1uz);
                }
                _reallocate();
            }
            void uninitialized_resize(std::size_t ns){
                _c = ns;
                _reallocate();
                _l = ns;
            }
            void resize(std::size_t ns,std::byte b=std::byte{0}){
                _c = ns;
                _reallocate();
                if(ns>*_l){
                    std::fill(*_m+*_l,*_m+ns,b);
                }
                _l = ns;
            }
            void clear(){
                _c = _l = 0;
                _reallocate();
            }
            void skip(std::size_t amnt){
                reserve(*_l += amnt);
            }
            constexpr std::byte& operator[](std::size_t ind) noexcept{
                return (*_m)[ind];
            }
            constexpr const std::byte& operator[](std::size_t ind) const noexcept{
                return (*_m)[ind];
            }
            constexpr std::byte* data() noexcept{
                return *_m;
            }
            constexpr const std::byte* data() const noexcept{
                return *_m;
            }
            using iterator = std::byte*;
            using const_iterator = const std::byte*;
            constexpr iterator begin() noexcept{
                return *_m;
            }
            constexpr const_iterator cbegin() const noexcept{
                return *_m;
            }
            constexpr const_iterator begin() const noexcept{
                return *_m;
            }
            constexpr iterator end() noexcept{
                return *_m+*_l;
            }
            constexpr const_iterator cend() const noexcept{
                return *_m+*_l;
            }
            constexpr const_iterator end() const noexcept{
                return *_m+*_l;
            }
            constexpr std::size_t size() const noexcept{
                return *_l;
            }
            constexpr std::size_t capacity() const noexcept{
                return *_c;
            }
            void shrink_to_fit(){
                if(*_l<*_c){
                    _c = *_l;
                    _reallocate();
                }
            }
            ~bytes(){
                delete[] *_m;
            }
    };
}
