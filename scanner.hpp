#pragma once
#include"bfile.hpp"
#include<utility>
#include<bitset>
namespace cppp{
    using ascii_mask = std::bitset<128uz>;
    template<std::size_t BUF=1024uz>
    class Scanner{
        BinaryFile bf;
        std::array<char8_t,BUF> pbuf;
        std::size_t bufn = 0uz; // replace both with inplace_vector when it comes out
        std::size_t bufp = 0uz;
        void popbuff(){
            if(bufp == bufn){
                bufp = 0uz;
                bufn = bf.read(std::as_writable_bytes(std::span(pbuf)));
                if(!bufn) throw std::logic_error("Scanner::next(): no more data");
            }
        }
        public:
            Scanner(BinaryFile&& bf) : bf(std::move(bf)){}
            char8_t next(){
                popbuff();
                return pbuf[bufp++];
            }
            void unchecked_next(){
                ++bufp;
            }
            char8_t peek(){
                popbuff();
                return pbuf[bufp];
            }
            bool expect(char8_t c){
                bool expected = (peek() == c);
                if(expected){
                    unchecked_next();
                }
                return expected;
            }
            bool expect(const ascii_mask& mask){
                bool expected = mask[peek()];
                if(expected){
                    unchecked_next();
                }
                return expected;
            }
            void skip(const ascii_mask& mask){
                while(expect(mask));
            }
    };
}
