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

#include <cstdint>
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


//  "is_close"
//
/// @fn template        <std::floating_point<T>>
///                     [[nodiscard]] inline bool
///                     is_close(const T a,
///                              const T b,
///                              T rel_tol = static_cast<T>(1e-9),
///                              T abs_tol = static_cast<T>(0))
/// @brief              Determine if two floating-point values are approximately equal (follows the "math.isclose()" Python function).
///
/// @tparam T           A floating-point type (e.g. float, double).
/// @param a            First value to compare.
/// @param b            Second value to compare.
/// @param rel_tol      Relative tolerance (must be ≥ 0).  Defaults value varies by \p T (see above)
/// @param abs_tol      Absolute tolerance (must be ≥ 0).  Default value is 0.
/// @return             true if \p a and \p b satisfy
///                     \f$|a - b| \le \max(\text{rel\_tol}\cdot\max(|a|,|b|),\,\text{abs\_tol})\f$;
///                     false if they differ by more than that or if either is NaN.
///
/// @throw              std::invalid_argument if \p rel_tol or \p abs_tol is negative.
/// @see                cblib::default_rel_tol_v
//
template <std::floating_point T>
[[nodiscard]] inline constexpr bool is_close( const T a,                            const T b,
                                              T rel_tol = default_rel_tol<T>(),     T abs_tol = static_cast<T>(0) )
{
    //  CASE 0 :    INVALID USE OF THIS TEMPLATE...
    if ( (rel_tol < T(0))  ||  (abs_tol < T(0)) )
    {
        if ( std::is_constant_evaluated() )     { return false; }       //  constexpr-safe: no exceptions in constant evaluation.
        throw std::invalid_argument("rel_tol and abs_tol must be non-negative");
    }


    //      1.      Early Exit for EXACT Equality (handles the cases of: "+0.0 vs. -0.0",  and  "+/- INF").
    if ( a == b )                               { return true; }

    //      2.      HANDLE NaN ARGUMENTS (NaNs are NEVER close).
    if ( std::isnan(a) || std::isnan(b) )       { return false; }

    //      3.      MIMIC BEHAVIOR OF THE PYTHON  "math.is_close(...)"  IMPLEMENTATION.
    if ( std::isinf(a) || std::isinf(b) )       { return false; }
    
    T       diff            = std::abs(a - b);
    T       scale           = std::max(std::abs(a), std::abs(b));
    T       bound           = std::max(rel_tol * scale, abs_tol);
    
    return (diff <= bound);
}


//  "tolerance_interval"
//
template<typename T>
[[nodiscard]] inline constexpr std::pair<T,T>
tolerance_interval( const T  ref,                      // reference “b”
                    T        rel_tol = static_cast<T>(1e-9),
                    T        abs_tol = static_cast<T>(0) )
{
    //  CASE 0 :    ASSERTIONS / EXCEPTIONS IF INVALID USAGE...
    static_assert( std::is_floating_point_v<T>,     "\"tolerance_interval\" requires a floating-point type" );
    if ( rel_tol < T(0) || abs_tol < T(0) )         { throw std::invalid_argument("rel_tol and abs_tol must be non-negative"); }
    if ( std::isnan(ref) || std::isinf(ref) )       { return {ref, ref}; }

    const T     base        = std::max(abs_tol, rel_tol * std::abs(ref));
    T           low         = ref - base;            // left endpoint so far
    T           high        = ref + base;            // right endpoint so far


    //  --- asymmetric extension when the *other* number’s magnitude dominates ---
    //      (rel_tol > 0 and moving *away* from zero on ref’s sign side)
    if ( rel_tol > T(0) && rel_tol < T(1) )
    {
        const T     one_minus   = T(1) - rel_tol;

        if ( ref > T(0) ) {                      //  positive reference  ===>  extend HIGH
            const T high_rel    = ref / one_minus;          // equality at |x| > |ref|
            high                = std::max(high, high_rel);
        }
        else if ( ref < T(0) ) {             // negative reference  ===>  extend LOW
            const T low_rel     = ref / one_minus;   // more negative endpoint
            low                 = std::min(low,  low_rel);
        }
        // ref == 0     : interval is symmetric; base is already correct
    }

    return { low, high };
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
