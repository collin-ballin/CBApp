/***********************************************************************************
*
*       ********************************************************************
*       ****            _ I N T E R N A L . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      October 1, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_MATH_INTERNAL_H
#define _CBLIB_MATH_INTERNAL_H 1

#include <iostream>
#include <type_traits>
#include <algorithm>
#if __cpp_concepts >= 201907L
# include <concepts>
#endif  //  C++20.  //

#include <cstdint>
#include <cassert>

#include <cmath>
#include <bit>          // std::bit_cast
#include <cstddef>
#include <iomanip>

#include <vector>
#include <array>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	//  C++11.  //



namespace cblib { namespace math { namespace numerics {   //     BEGINNING NAMESPACE "cblib::math::numerics"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      NUMERICAL UTILITIES / INTERNAL IMPLEMENTATIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//              1.0.    TYPES.
// *************************************************************************** //

//  "RoundingMode"
//
enum class RoundingMode : uint8_t
{
      ties_to_even = 0                //  banker’s rounding
    , toward_zero               //  truncate
    , toward_pos_inf            //  ceil
    , toward_neg_inf            //  floor
    , away_from_zero
    , COUNT
};
//
constexpr RoundingMode                      DEF_DEFAULT_ROUNDING_MODE                       = RoundingMode::ties_to_even;






// *************************************************************************** //
//              1.1.    "is_close" UTILITIES.
// *************************************************************************** //

//  "default_rel_tol_v"
//
/// @decl           template<std::floating_point T> inline constexpr T default_rel_tol_v
/// @brief          Precision-aware default relative tolerance used with \ref is_close.
///
/// @tparam T       A floating-point type (e.g. float, double).
/// @var            default_rel_tol_v<T>
///
/// @details        Expands to yield a value that is appropriate for the precision afforded by each floating-point type parameter \p T :
///                 - \c float          = \f$10^{-6}\f$
///                 - \c double         = \f$10^{-9}\f$
///                 - \c long\ double   = \f$10^{-12}\f$
/// @note           This value is intended as the default for the \p rel_tol parameter of \ref is_close
///                 to provide reasonable behavior across different floating-point precisions.
/// @see            is_close
//
template <std::floating_point T>
[[nodiscard]] constexpr T default_rel_tol(void) noexcept {
    if constexpr      ( std::same_as<T, float> )            { return T(1e-6);   }   //  "float"             : 1e-6.
    else if constexpr ( std::same_as<T, double> )           { return T(1e-9);   }   //  "double"            : 1e-9.
    else if constexpr ( std::same_as<T, long double> )      { return T(1e-12);  }   //  "long double"       : 1e-12.
//
    else                                                    { return T(1e-6);   }   //  DEFAULT             : 1e-6.
}






// *************************************************************************** //
//              1.2.    "ROUND_TO" UTILITIES.
// *************************************************************************** //

template<auto N, typename T>
struct round_to_IMPL {
    static T apply(T v) {
        static_assert(std::is_floating_point_v<T>,
                      "round_to only supports floating-point types");
        T base = std::pow (T(10), N);
        return std::ceil(v * base) / base;
    }
};

//  — float specialization —
template<auto N>
struct round_to_IMPL<N, float> {
    static float apply(float v) {
        float base = std::powf(10.0f, static_cast<int>(N));
        return std::ceilf(v * base) / base;
    }
};

//  — double specialization —
template<auto N>
struct round_to_IMPL<N, double> {
    static double apply(double v) {
        double base = std::pow (10.0, static_cast<int>(N));
        return std::ceil (v * base) / base;
    }
};

//  — long double specialization —
template<auto N>
struct round_to_IMPL<N, long double> {
    static long double apply(long double v) {
        long double base = std::powl(10.0L, static_cast<long>(N));
        return std::ceill(v * base) / base;
    }
};






// *************************************************************************** //
//              1.3.    "QUANTIZE" UTILITIES.
// *************************************************************************** //

//  "_is_finite"
//
template <std::floating_point T>
[[nodiscard]] inline constexpr bool         _is_finite              (T x) noexcept          { return std::isfinite(x); }


//  "_round_integral"
//      Round a real n to an integer-valued real according to mode.
//
template <std::floating_point T>
[[nodiscard]] inline T                      _round_integral         (T n, RoundingMode mode) noexcept
{
    switch (mode)
    {
        case RoundingMode::toward_zero      :       { return std::trunc(n);                                 }
        case RoundingMode::toward_pos_inf   :       { return std::ceil(n);                                  }
        case RoundingMode::toward_neg_inf   :       { return std::floor(n);                                 }
        case RoundingMode::away_from_zero   :       { return (n >= T(0)) ? std::ceil(n) : std::floor(n);    }
    //
        case RoundingMode::ties_to_even     :
    //
        default                             :
        {
            //  Half-even without touching global fenv:
            //  i = floor(n), f = n - i ∈ [0,1)
            const T i = std::floor(n);
            const T f = n - i;
            
            if ( f < T(0.5) )               { return i;         }
            if ( f > T(0.5) )               { return i + T(1);  }
            //  tie: pick the even integer
            //  i is integer-valued; even if remainder is exactly 0.5.
            //  Determine parity by checking i mod 2 via nearby integral arithmetic.
            const T half_even = std::fmod(i, T(2)) == T(0) ? i : (i + T(1));
            return half_even;
        }
    }
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  UTILITIES".






// *************************************************************************** //
//
//
//
//      2.      IEEE-754 STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "compare_IEEE754"
//
template <std::floating_point T>
[[nodiscard]] inline constexpr bool compare_IEEE754(T a, T b) noexcept
{
    //  Compare raw IEEE-754 payloads without using FP equality.
    const auto      pa      = std::bit_cast< std::array<std::byte, sizeof(T)> >( a );
    const auto      pb      = std::bit_cast< std::array<std::byte, sizeof(T)> >( b );
    
    return pa == pb;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2.  IEEE-754".












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} } }//   END OF "cblib::math::numerics".







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_INTERNAL_H  //
