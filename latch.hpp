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
#include"eto.hpp"
#include<system_error>
#include<type_traits>
#include<sys/mman.h>
#include<unistd.h>
#include<cstddef>
#include<cstdint>
#include<climits>
#include<cstring>
#include<string>
#include<cerrno>
#include<ranges>
#include<array>
#include<meta>
#if CHAR_BIT != 8
#error Only works on 8-bit byte systems
#endif
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
    template<typename T>
    concept usable_in_basic_latch = sizeof(T) == 4uz && std::is_trivially_copyable_v<T>;
    template<usable_in_basic_latch T>
    class basic_latch_handle{
        volatile std::byte* m;
        basic_latch_handle(volatile std::byte* b) noexcept : m(b){}
        template<usable_in_basic_latch U,template_string name>
        friend struct basic_latch;
        public:
            void set(T value) noexcept{
                std::array<std::byte,sizeof(T)> src{std::bit_cast<std::array<std::byte,sizeof(T)>>(value)};
                std::ranges::copy(src,m);
                detail::serialize();
            }
            T observe() const noexcept{
                std::array<std::byte,sizeof(T)> rd;
                std::ranges::copy_n(m,rd.size(),rd.begin());
                return std::bit_cast<T>(rd);
            }
    };
    namespace detail{
        inline std::uintptr_t page_size() noexcept{
            static std::uintptr_t size = static_cast<std::uintptr_t>(sysconf(_SC_PAGESIZE));
            return size;
        }
    }
    template<usable_in_basic_latch T,template_string name>
    struct basic_latch{
        template<T init>
        static T get() noexcept{
            using namespace std::literals;
            T retv;
            asm volatile((detail::static_asm_c_string_view(name.sv()+u8":\nmovl %[init], %[dst]"s)) : [dst] "=r"(retv) : [init] "i"(init));
            return retv;
        }
        static T get() noexcept requires(std::is_default_constructible_v<T>){
            return get<T{}>();
        }
        static void set(T value) noexcept{
            using namespace std::literals;
            asm((detail::static_asm_c_string_view(u8"movl %[val], 1 + "s+name.sv()+u8"(%%rip)"sv)) : : [val] "ri"(value));
        }
        static T observe() noexcept{
            using namespace std::literals;
            T retv;
            asm((detail::static_asm_c_string_view(u8"movl 1 + "s+name.sv()+u8"(%%rip), %[dst]"sv)) : [dst] "=r"(retv));
            return retv;
        }
        static basic_latch_handle<T> handle(){
            using namespace std::literals;
            volatile std::byte* memp;
            asm((detail::static_asm_c_string_view(u8"leaq 1 + "s+name.sv()+u8"(%%rip), %[res]"sv)) : [res] "=r"(memp));
            static bool unlocked = false;
            if(!unlocked){
                std::uintptr_t ps = detail::page_size();
                std::uintptr_t page_begin = reinterpret_cast<std::uintptr_t>(memp)&-ps;
                if(mprotect(reinterpret_cast<void*>(page_begin),ps,PROT_WRITE|PROT_EXEC)){
                    throw std::system_error(errno,std::system_category());
                }
                unlocked = true;
            }
            return memp;
        }
    };
}
