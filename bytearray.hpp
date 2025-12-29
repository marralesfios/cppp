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
            bytes() = default;
            bytes(bytes&&) = default;
            bytes& operator=(bytes&&) = default;
            bytes(std::initializer_list<std::byte> b){
                std::copy_n(b.begin(),b.size(),append(b.size()));
            }
            bool empty() const{
                return *_l == 0uz;
            }
            void append(frozenbuffer b){
                std::copy_n(b.data(),b.size(),append(b.size()));
            }
            std::byte* append(std::size_t n){
                std::byte* p = end();
                std::size_t _ll = *_l+n;
                resize(_ll);
                _l = _ll;
                return p;
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
            std::byte& operator[](std::size_t ind){
                return (*_m)[ind];
            }
            const std::byte& operator[](std::size_t ind) const{
                return (*_m)[ind];
            }
            std::byte* data(){
                return *_m;
            }
            const std::byte* data() const{
                return *_m;
            }
            using iterator = std::byte*;
            using const_iterator = const std::byte*;
            iterator begin(){
                return *_m;
            }
            const_iterator cbegin() const{
                return *_m;
            }
            const_iterator begin() const{
                return *_m;
            }
            iterator end(){
                return *_m+*_l;
            }
            const_iterator cend() const{
                return *_m+*_l;
            }
            const_iterator end() const{
                return *_m+*_l;
            }
            std::size_t size() const{
                return *_l;
            }
            std::size_t capacity() const{
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
