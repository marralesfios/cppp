#pragma once
#include"vector.hpp"
#include<concepts>
#include<cmath>
namespace cppp{
    namespace detail{
        template<typename F>
        constexpr vec3<F> sclamp(vec3<F> vec,F down,F up) noexcept{
            return max(min(vec,up),down);
        }
    }
    
    // RGB range: [0,1]
    // HSV range: S,V [0,1], H [0,6)
    
    template<std::floating_point F>
    [[gnu::optimize("-ffast-math")]] constexpr vec3<F> rgb_to_hsv(vec3<F> rgb) noexcept{
        F max = std::max(rgb.x(),std::max(rgb.y(),rgb.z()));
        F min = std::min(rgb.x(),std::min(rgb.y(),rgb.z()));
        F hue;
        F sat;
        if(F chroma = max-min){
            if(max == rgb.x()){
                hue = (rgb.y()-rgb.z())/chroma;
                if(rgb.y() < rgb.z()) hue += static_cast<F>(6);
            }else if(max == rgb.y()){
                hue = (rgb.z()-rgb.x())/chroma + static_cast<F>(2);
            }else{
                hue = (rgb.x()-rgb.y())/chroma + static_cast<F>(4);
            }
            sat = chroma / max; // chroma > 0 && min >= 0 => max > 0
        }else{
            hue = F();
            sat = F();
        }
        return {hue,sat,max};
    }
    template<std::floating_point F>
    [[gnu::optimize("-ffast-math")]] constexpr vec3<F> hsv_to_rgb_parallel(vec3<F> hsv) noexcept{
        vec3<F> hues{hsv.x()};
        vec3<F> saturations{hsv.y()};
        vec3<F> values{hsv.z()};
        vec3<F> k = fmod(hues+vec3<F>{static_cast<F>(5),static_cast<F>(3),static_cast<F>(1)},static_cast<F>(6));
        return values * (static_cast<F>(1) - saturations * detail::sclamp(min(k,static_cast<F>(4)-k),F(),static_cast<F>(1)));
    }
    template<std::floating_point F>
    [[gnu::optimize("-ffast-math")]] constexpr vec3<F> hsv_to_rgb_piecewise(vec3<F> hsv) noexcept{
        F chroma = hsv.z() * hsv.y();
        F other = chroma * (static_cast<F>(1) - std::abs(std::fmod(hsv.x(),static_cast<F>(2)) - static_cast<F>(1)));
        vec3<F> base;
        switch(static_cast<std::uint8_t>(hsv.x())){
            case 0: base = {chroma,other,F()}; break;
            case 1: base = {other,chroma,F()}; break;
            case 2: base = {F(),chroma,other}; break;
            case 3: base = {F(),other,chroma}; break;
            case 4: base = {other,F(),chroma}; break;
            case 5: base = {chroma,F(),other}; break;
        }
        return base + hsv.z() * (static_cast<F>(1) - hsv.y());
    }
}
