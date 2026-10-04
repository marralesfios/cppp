#pragma once
#include<meta>
#include"functional.hpp"
namespace cppp{
    template<std::meta::info field> requires(is_nonstatic_data_member(field))
    struct accesses{
        private:
            using parameter_type = [:parent_of(field):];
            using result_type = [:type_of(field):];
        public:
            constexpr static result_type& operator()(parameter_type& obj) noexcept{
                return obj.[:field:];
            }
            constexpr static const result_type& operator()(const parameter_type& obj) noexcept{
                return obj.[:field:];
            }
    };
    template<std::meta::info field> requires(is_nonstatic_data_member(field))
    struct forwarding_accesses{
        private:
            using parameter_type = [:parent_of(field):];
            using result_type = [:type_of(field):];
        public:
            constexpr static result_type& operator()(parameter_type& obj) noexcept{
                return obj.[:field:];
            }
            constexpr static const result_type& operator()(const parameter_type& obj) noexcept{
                return obj.[:field:];
            }
            constexpr static result_type&& operator()(parameter_type&& obj) noexcept{
                return std::move(obj.[:field:]);
            }
            constexpr static const result_type&& operator()(const parameter_type&& obj) noexcept{
                return std::move(obj.[:field:]);
            }
    };
    template<std::meta::info method> requires(is_class_member(method) && is_function(method) && !is_special_member_function(method))
    struct invokes_member{
        private:
            using object_type = [:parent_of(method):];
        public:
            template<typename ...A>
            constexpr static decltype(auto) operator()(object_type& obj,A&& ...a) noexcept(noexcept(obj.[:method:](std::forward<A>(a)...))){
                return obj.[:method:](std::forward<A>(a)...);
            }
            template<typename ...A>
            constexpr static decltype(auto) operator()(const object_type& obj,A&& ...a) noexcept(noexcept(obj.[:method:](std::forward<A>(a)...))){
                return obj.[:method:](std::forward<A>(a)...);
            }
            template<typename ...A>
            constexpr static decltype(auto) operator()(object_type&& obj,A&& ...a) noexcept(noexcept(obj.[:method:](std::forward<A>(a)...))){
                return std::move(obj).[:method:](std::forward<A>(a)...);
            }
            template<typename ...A>
            constexpr static decltype(auto) operator()(const object_type&& obj,A&& ...a) noexcept(noexcept(obj.[:method:](std::forward<A>(a)...))){
                return std::move(obj).[:method:](std::forward<A>(a)...);
            }
    };
}
