#pragma once
#include"freelist.hpp"
#include<utility>
#include<cstdint>
#include<memory>
#include<vector>
#include<new>
namespace cppp{
    template<typename T>
    class indexed_array_pool{
        freelist<std::uint64_t> free;
        T* data;
        std::uint64_t bufsize;
        [[no_unique_address]] std::allocator<T> alloc;
        void delete_buffer(){
            if(data){
                for_each_index([this](std::uint64_t i){
                    data[i].~T();
                });
                alloc.deallocate(data,bufsize);
            }
        }
        public:
            constexpr indexed_array_pool() : data(nullptr), bufsize(0){}
            indexed_array_pool(const indexed_array_pool&) = delete;
            constexpr indexed_array_pool(indexed_array_pool&& other) noexcept : free(std::move(other)), data(std::exchange(other.data,nullptr)), bufsize(std::exchange(other.bufsize,0)){}
            indexed_array_pool operator=(const indexed_array_pool&) = delete;
            indexed_array_pool& operator=(indexed_array_pool&& other) noexcept{
                delete_buffer();
                free = std::move(other.free);
                data = std::exchange(other.data,nullptr);
                return *this;
            }
            template<typename ...A>
            std::uint64_t emplace(A&& ...a) noexcept(std::is_nothrow_move_constructible<T> || std::is_nothrow_copy_constructible_v<T>){
                std::uint64_t i = free.allocate();
                if(i>bufsize){
                    std::uint64_t nbufsize = bufsize*2;
                    T* ndata = alloc.allocate(nbufsize);
                    if constexpr(std::is_nothrow_move_constructible_v<T>){
                        for_each_index([this,ndata](std::uint64_t i) noexcept{
                            new(ndata+i) T(std::move(data[i]));
                        });
                    }else if constexpr(std::is_nothrow_copy_constructible_v<T>){
                        for_each_index([this,ndata](std::uint64_t i) noexcept{
                            new(ndata+i) T(data[i]);
                        });
                    }else{
                        std::vector<T*> constructed;
                        try{
                            for_each_index([this,ndata,&constructed](std::uint64_t i) noexcept{
                                new(ndata+i) T(std::move(data[i]));
                                constructed.emplace_back(ndata+i);
                            });
                        }catch(...){
                            for(T* const co : constructed){
                                co->~T();
                            }
                            alloc.deallocate(ndata,nbufsize);
                            throw;
                        }
                    }
                    alloc.deallocate(std::exchange(data,ndata),std::exchange(bufsize,nbufsize));
                }
                new(data+i) T(std::forward<A>(a)...);
                return i;
            }
            void pop(std::size_t i){
                free.deallocate(i);
                data[i].~T();
            }
            constexpr T& operator[](std::size_t i) noexcept{
                return data[i];
            }
            constexpr const T& operator[](std::size_t i) const noexcept{
                return data[i];
            }
            template<typename F>
            constexpr void for_each_index(const F& fn) noexcept(noexcept(fn(std::declval<std::uint64_t>()))){
                auto hole = free.holes().begin();
                const auto last_hole = free.holes().end();
                for(std::uint64_t i=0;i<free.size();++i){
                    if(hole != last_hole && i == *hole){
                        ++hole;
                        continue;
                    }
                    fn(i);
                }
            }
            ~indexed_array_pool(){
                delete_buffer();
            }
    };
}
