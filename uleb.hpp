#pragma once
#include<concepts>
#include"object-view.hpp"
#include"bytearray.hpp"
namespace cppp{
    // TODO: Standard ULEB support
    
    // Custom version of ULEB128, highest bit of each byte is the opposite of its normal value
    template<std::unsigned_integral T>
    T muleb128_r(frozen_byte_view& b){
        T r = 0;
        std::byte v;
        std::uint16_t n = 0;
        do{
            v = b.pop_front();
            // force narrow if promotion happened
            r = static_cast<T>(r | (static_cast<T>(v&0x7f_b) << n));
            n += 7;
        }while((v&0x80_b) == 0_b);
        return r;
    }
    template<std::unsigned_integral T>
    void muleb128_w(bytes& dst,T v){
        do{
            dst.append(static_cast<std::byte>(v)&0x7f_b);
            v >>= 7;
        }while(v);
        dst[dst.size()-1] |= 0x80_b;
    }
}
