#pragma once
#ifndef __GNUC__
#error Requires GCC-compatible compiler
#endif
#ifndef __x86_64__
#error Only works on x86_64 architecture
#endif
#if !__has_include(<unistd.h>) || !__has_include(<sys/mman.h>)
#error requires POSIX
#endif
#include"template-string.hpp"
#include<system_error>
#include<sys/mman.h>
#include<unistd.h>
#include<cstddef>
#include<cstdint>
#include<cstring>
#include<string>
#include<cerrno>
#include<ranges>
#include<array>
#include<meta>
namespace cppp{
    namespace detail{
        #ifdef __SERIALIZE__
        #include<x86gprintrin.h>
        inline void serialize() noexcept{
            _serialize();
        }
        #else
        inline void serialize() noexcept{
            asm("xor %%eax, %%eax\ncpuid" : : : "cc", "rax", "rbx", "rcx", "rdx");
        }
        #endif
        consteval std::string_view static_asm_c_string_view(std::u8string_view usv){
            std::string s{std::from_range,usv};
            return std::string_view(std::define_static_array(s));
        }
    }
    template<template_string name>
    std::uint32_t dword_latch(){
        using namespace std::literals;
        std::uint32_t retv;
        asm volatile((detail::static_asm_c_string_view(name.sv()+u8":\nmovl $0, %[dst]"s)) : [dst] "=r"(retv));
        return retv;
    }
    class DwordLatchHandle{
        volatile std::byte* m;
        static std::uintptr_t page_size() noexcept{
            static std::uintptr_t size = static_cast<std::uintptr_t>(sysconf(_SC_PAGESIZE));
            return size;
        }
        DwordLatchHandle(volatile std::byte* b) : m(b){}
        public:
            void set(std::uint32_t value){
                std::array<std::byte,4uz> src{std::bit_cast<std::array<std::byte,4uz>>(value)};
                std::ranges::copy(src,m);
                detail::serialize();
            }
            template<template_string name>
            static DwordLatchHandle get_latch(){
                using namespace std::literals;
                volatile std::byte* memp;
                asm((detail::static_asm_c_string_view(u8"leaq "s+name.sv()+u8"(%%rip), %[res]"sv)) : [res] "=r"(memp));
                ++memp;
                static bool unlocked = false;
                if(!unlocked){
                    std::uintptr_t page_begin = reinterpret_cast<std::uintptr_t>(memp)&-page_size();
                    if(mprotect(reinterpret_cast<void*>(page_begin),page_size(),PROT_WRITE|PROT_EXEC)){
                        throw std::system_error(errno,std::system_category());
                    }
                    unlocked = true;
                }
                return memp;
            }
    };
}
