/***********************************************************************************
*
*       ********************************************************************
*       ****             _ U T I L I T Y . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 13, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_MATH_UTILITY_H
#define _CBLIB_MATH_UTILITY_H 1

#include <iostream>
#include <type_traits>
#include <algorithm>
#if __cpp_concepts >= 201907L
# include <concepts>
#endif  //  C++20.  //

#include <cmath>
#include <cstddef>
#include <iomanip>

#include <vector>
#include <array>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	//  C++11.  //



namespace cblib { namespace math {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //



//  1.  GENERIC / GENERAL MATH STUFF...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1.1     "round_to" Function.
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


/// @fn [[nodiscard]] inline T round_to(const T value)
/// @brief Rounds \p value *up* to \p N decimal places (or to the nearest power of ten if \p N is negative).
///
/// @tparam N           The power‐of‐ten exponent for scaling (non‐type template parameter).
/// @tparam T           A floating‐point type (e.g. float, double, long double).
/// @param value        The value to round.
/// @return             The smallest \p T not less than \p value with \p N decimal digits, computed as  
///                     \f$\displaystyle\frac{\lceil\,\text{value}\times10^{N}\,\rceil}{10^{N}}\f$.
template<auto N, typename T>
inline T round_to(T v) {
    return round_to_IMPL<N, T>::apply(v);
}


// *************************************************************************** //
//
//
//
// *************************************************************************** //
//      1.2     "is_close" Function.
// *************************************************************************** //

/// @tparam T           A floating-point type (e.g. float, double).
/// @fn [[nodiscard]]   bool is_close(const T a,
///                                 const T b,
///                                 T rel_tol = static_cast<T>(1e-9),
///                                 T abs_tol = static_cast<T>(0))
/// @brief Determines whether two floating-point values are approximately equal.
///
/// @tparam T           A floating-point type (e.g. float, double).
/// @param a            First value to compare.
/// @param b            Second value to compare.
/// @param rel_tol      Relative tolerance (must be ≥ 0). Default is 1e-9.
/// @param abs_tol      Absolute tolerance (must be ≥ 0). Default is 0.
///
/// @return             true if \p a and \p b satisfy
///                     \f$|a - b| \le \max(\text{rel\_tol}\cdot\max(|a|,|b|),\,\text{abs\_tol})\f$;
///                     false if they differ by more than that or if either is NaN.
///
/// @throw std::invalid_argument if \p rel_tol or \p abs_tol is negative.
template<typename T>
[[nodiscard]] inline constexpr bool is_close( const T a,                            const T b,
                                              T rel_tol = static_cast<T>(1e-9),     T abs_tol = static_cast<T>(0) )
{
    //  CASE 0 :    ASSERTIONS / EXCEPTIONS IF INVALID USAGE...
    static_assert( std::is_floating_point_v<T>,     "\"is_close\" requires a floating-point type" );
    if ( rel_tol < T(0) || abs_tol < T(0) )         { throw std::invalid_argument("rel_tol and abs_tol must be non-negative"); }


    //      1.      Early Exit for EXACT Equality ( also handles +/- INF ).
    if ( a == b )                               { return true; }

    //      2.      HANDLE NaN ARGUMENTS.
    if ( std::isnan(a) || std::isnan(b) )       { return false; }

    //      3.      MIMIC BEHAVIOR OF THE PYTHON  "math.is_close(...)"  IMPLEMENTATION.
    //  if ( std::isinf(a) || std::isinf(b) )       { return false; }
    T       diff            = std::abs(a - b);
    T       scale           = std::max(std::abs(a), std::abs(b));
    T       bound           = std::max(rel_tol * scale, abs_tol);
    
    return (diff <= bound);
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "math" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_UTILITY_H  //
