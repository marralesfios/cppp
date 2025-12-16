#pragma once
#include<initializer_list>
#include<type_traits>
#include<algorithm>
#include<cstdint>
#include<utility>
#include<climits>
#include<cmath>
#include<span>
#include<cppp/zeroing-field.hpp>
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
        template<typename T>
        struct _expandbytes{};
        template<std::size_t... index>
        struct _expandbytes<std::integer_sequence<std::size_t,index...>>{
            template<typename I>
            static void write(std::byte* memory,I number){
                (..., (
                    *(memory+index) = static_cast<std::byte>(number >> (index*CHAR_BIT))
                ));
            }
        };
        public:
            bytes() = default;
            bytes(bytes&&) = default;
            bytes& operator=(bytes&&) = default;
            bytes(std::initializer_list<std::byte> b){
                append(b);
            }
            bool empty() const{
                return *_l == 0uz;
            }
            template<typename I> requires(std::is_same_v<I,std::byte> || (std::is_integral_v<I> && std::is_unsigned_v<I>))
            void writel(std::size_t at,I num){
                _expandbytes<std::make_index_sequence<sizeof(I)>>::write(*_m+at,num);
            }
            void write(std::size_t at,std::span<const std::byte> m){
                std::ranges::copy(m,*_m+at);
            }
            void write(std::size_t at,std::byte b){
                (*_m)[at] = b;
            }
            template<typename I>
            void writel_and_move(std::size_t& at,I num){
                writel<I>(at,num);
                at += sizeof(I);
            }
            void write_and_move(std::size_t& at,std::span<const std::byte> m){
                write(at,m);
                at += m.size();
            }
            void write_and_move(std::size_t& at,std::byte b){
                write(at,b);
                ++at;
            }
            template<typename I>
            void appendl(I num){
                std::size_t _ll = *_l+sizeof(I);
                reserve(_ll);
                writel<I>(*_l,num);
                _l = _ll;
            }
            void append(std::span<const std::byte> b){
                std::size_t _ll = *_l+b.size();
                reserve(_ll);
                write(*_l,b);
                _l = _ll;
            }
            void append(std::byte b){
                appendl<std::byte>(b);
            }
            void append(std::uint8_t b){
                append(std::byte{b});
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
