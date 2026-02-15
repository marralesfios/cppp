#pragma once
namespace cppp{
    [[noreturn]] consteval void consteval_trap(){
        [[assume(false)]]; // UB disallowed at compile time
    }
    consteval void consteval_assert(bool v){
        if(!v) consteval_trap();
    }
}
