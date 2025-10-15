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



namespace cblib { namespace math {   //     BEGINNING NAMESPACE "cblib::math"...
// *************************************************************************** //
// *************************************************************************** //



namespace numerics {   //     BEGINNING NAMESPACE "numerics"...
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


//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.0.  "TYPES" ].






// *************************************************************************** //
//              1.1.    IEEE-754 STUFF.
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
// *************************************************************************** //
// *************************************************************************** //   END [ 1.1.  "IEEE-754" ].






// *************************************************************************** //
//              1.2.    "is_close" UTILITIES.
// *************************************************************************** //

//  "cv_default_rel_tol_v"
//
/// @decl           template<std::floating_point T> inline constexpr T cv_default_rel_tol_v_v
/// @brief          Precision-aware default relative tolerance used with \ref is_close.
///
/// @tparam T       A floating-point type (e.g. float, double).
/// @var            cv_default_rel_tol_v_v<T>
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
[[nodiscard]] constexpr T cv_default_rel_tol_v(void) noexcept {
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
/// @see                cblib::numerics::cv_default_rel_tol_v_v
//
template <std::floating_point T>
[[nodiscard]] inline constexpr bool is_close(   const T     a
                                              , const T     b
                                              , T           rel_tol = ::cblib::math::numerics::cv_default_rel_tol_v<T>()
                                              , T           abs_tol = static_cast<T>(0) )
{
    const bool  a_inf   = std::isinf(a),
                b_inf   = std::isinf(b);

    //      CASE 0 :    INVALID USE OF THIS TEMPLATE...
    if ( (rel_tol < T(0))  ||  (abs_tol < T(0)) )
    {
        if ( std::is_constant_evaluated() )     { return false; }       //  constexpr-safe: no exceptions in constant evaluation.
        throw std::invalid_argument("is_close: argument \"rel_tol\"  and  \"abs_tol\"  must be non-negative");
    }



    //      1.          NaNs ARE *NEVER* CLOSE...
    if ( std::isnan(a)  ||  std::isnan(b) )     { return false; }
 
    //      2.          EQUAL INF *ARE* CLOSE;   UNEQUAL INF *ARE NOT* CLOSE...
    if ( a_inf  ||  b_inf ) {
        if ( a_inf  &&  b_inf )     { return std::signbit(a) == std::signbit(b); }  //  Same sign infinities are considered equal (without FP ==).
        return false;
    }
    
    //      3.          BOTH CASES OF ZERO (INCLUDING +0.0  vs  -0.0) ARE CLOSE.    USE CLASSIFICATION INSTEAD OF "FLOAT == " EQUALITY...
    if ( (std::fpclassify(a) == FP_ZERO)  &&  (std::fpclassify(b) == FP_ZERO) )
        { return true; }

    //      4.          EXACT FINITE-BITWISE EQUALITY COMPARISON  [ AVOIDS "FLOAT == " ]...
    if ( ::cblib::math::numerics::compare_IEEE754(a, b) )      { return true; }

    //      5.          TOLERANCE EVALUATION.   MIMICS BEHAVIOR OF THE PYTHON  "math.is_close(...)"  IMPLEMENTATION...
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



//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.2.  "is_close" ].






// *************************************************************************** //
//              1.3.    "ROUND_TO" UTILITIES.
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


//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.3.  "round_to" ].






// *************************************************************************** //
//              1.4.    "QUANTIZE" UTILITIES.
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
            //  CASE 0 :    TIE --- pick the even integer using is_close instead of direct == on floats
            const T         i           = std::floor(n);
            const T         f           = n - i;

            if ( f < T(0.5) )           { return i;         }
            if ( f > T(0.5) )           { return i + T(1);  }
            
            
            //  parity test with tolerance...
            const T         r           = std::fmod(i, T(2));           //  r ∈ (-2, 2)
            const bool      even        = (
                                            is_close(r, T(0)    )   ||  //  Numerically equal-to 0.
                                            is_close(r, T(2)    )   ||  //  guard for +2 due to fp noise
                                            is_close(r, T(-2)   )       //  guard for -2 due to fp noise
                                        );
            return (even)    ? i     : (i + T(1));
        }
    }
}


//  "quantize"      [ CORE-IMPLEMENTATION ].
//
//      Quantize  x  onto the lattice,  { origin + k * quantum,  where k | k ∈ ℤ }.
//      Preconditions :     quanta > 0      (returns x unchanged if violated).
//                          NaN / ±inf      pass through unchanged.
//
template <std::floating_point T>
[[nodiscard]] inline T                      quantize_IMPL           (T x, T quanta, T origin, ::cblib::math::numerics::RoundingMode mode) noexcept
{
    using       namespace       ::cblib::math::numerics;

    //      1.  MAP TO INDEX-SPACE (Integer Multiples);  ROUND VALUE;  MAP BACK TO VALUE-SPACE...
    const T     n               = (x - origin) / quanta;        //  A.      n = (x - origin) / quanta
    const T     k               = _round_integral(n, mode);     //  B.      k = round(n)
    T           y               = origin + k * quanta;          //  C.      y = origin + k * quanta

    //      2.  PRESERVE SIGNED-ZERO BY USING  sign_of( x - origin )...
    if ( is_close(y, T(0)) )    { y = std::copysign(T(0), x - origin); }

    return y;
}


//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.4.  "quantize" ].



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "UTILITIES" ]].






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "numerics".






// *************************************************************************** //
//
//
//
//      3.      GENERAL MATHEMATICAL OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "GENERIC MATH" ]].






// *************************************************************************** //
//
//
//
//      X.      PUBLIC API FOR "UTILITY MATH"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//              X.1.    "round_to" API.
// *************************************************************************** //

//  "round_to"
//
/// @fn [[nodiscard]] inline T round_to(const T value)
/// @brief Rounds \p value *up* to \p N decimal places (or to the nearest power of ten if \p N is negative).
///
/// @tparam N           The power‐of‐ten exponent for scaling (non‐type template parameter).
/// @tparam T           A floating‐point type (e.g. float, double, long double).
/// @param value        The value to round.
/// @return             The smallest \p T not less than \p value with \p N decimal digits, computed as  
///                     \f$\displaystyle\frac{\lceil\,\text{value}\times10^{N}\,\rceil}{10^{N}}\f$.
//
template<auto N, typename T>
[[nodiscard]] inline T round_to(T v)
{
    return ::cblib::math::numerics::round_to_IMPL<N, T>::apply(v);
}



//
// *************************************************************************** //
// *************************************************************************** //   END [ X.1.  "round_to" ].






// *************************************************************************** //
//              X.2.    "is_close" API.
// *************************************************************************** //

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
/// @see                cblib::numerics::cv_default_rel_tol_v_v
//
template <std::floating_point T>
[[nodiscard]] inline constexpr bool is_close(   const T     a
                                              , const T     b
                                              , T           rel_tol = ::cblib::math::numerics::cv_default_rel_tol_v<T>()
                                              , T           abs_tol = static_cast<T>(0) )
{
    return numerics::is_close(a, b, rel_tol, abs_tol);
}

//  "tolerance_interval"
//
template <std::floating_point T>
[[nodiscard]] inline constexpr std::pair<T,T>
tolerance_interval( const T  ref,                      // reference “b”
                    T        rel_tol = static_cast<T>(1e-9),
                    T        abs_tol = static_cast<T>(0) )
{
    return numerics::tolerance_interval(ref, rel_tol, abs_tol);
}



//
// *************************************************************************** //
// *************************************************************************** //   END [ X.2.  "is_close" ].






// *************************************************************************** //
//              X.3.    "quantize" API.
// *************************************************************************** //

//  "quantize"      [ WRAPPER-IMPLEMENTATIONS ].
//
template <std::floating_point T, std::floating_point U>
[[nodiscard]] inline T                      quantize                (   T                       x
                                                                      , U                       quanta
                                                                      , U                       origin  = U(0)
                                                                     , ::cblib::math::numerics::RoundingMode  mode    = ::cblib::math::numerics::DEF_DEFAULT_ROUNDING_MODE ) noexcept
{
    using       namespace   ::cblib::math::numerics;
    if ( !_is_finite(x) || !_is_finite(quanta) || !_is_finite(origin) || !(quanta > T(0)) )   { return x; }
    return quantize_IMPL( x, static_cast<T>(quanta), static_cast<T>(origin), mode );
}


//  "quantize_inplace"
//      "in-place" overload     [ returns the value via an OUT-PARAM ]...
//
template <std::floating_point T>
inline void                                 quantize_inplace        (   T &                     x
                                                                      , T                       quantum
                                                                      , T                       origin      = T(0)
                                                                     , ::cblib::math::numerics::RoundingMode  mode        = ::cblib::math::numerics::DEF_DEFAULT_ROUNDING_MODE ) noexcept
{
    x = quantize(x, quantum, origin, mode);
}


//  "quantize_decimals"
//
//      Quantize x to a given number of decimal digits.
//      digits ≥ 0 → snap to 10^{-digits} (e.g., 3 → 0.001)
//      digits < 0 → snap to powers of 10 (e.g., -1 → 10, -2 → 100)
//
template<std::floating_point T>
[[nodiscard]] inline T                      quantize_decimals       (   T                       x
                                                                      , int                     digits
                                                                      , T                       origin      = T(0)
                                                                      , ::cblib::math::numerics::RoundingMode  mode        = ::cblib::math::numerics::DEF_DEFAULT_ROUNDING_MODE ) noexcept
{
    using           namespace       ::cblib::math::numerics;
    constexpr int   MAX_DEC         = 308; // ~double range; ok for float/ldbl as a soft guard
    
    //  Using std::pow at runtime; clamp extremes to avoid 0/inf quantum.
    //  Note: for very large |digits|, pow may under/overflow; we guard mildly.
    if ( digits >  MAX_DEC )        { digits =  MAX_DEC; }
    if ( digits < -MAX_DEC )        { digits = -MAX_DEC; }

    const T         q               = std::pow(T(10), T(-digits)); // works for ±digits
    return quantize(x, q, origin, mode);
}


//  "quantize_decimals_inplace"
//      "in-place" overload of "quantize"   [ returns the value via an OUT-PARAM ]...
//
template <std::floating_point T>
inline void                                 quantize_decimals_inplace   ( T &                       x
                                                                          , int                     digits
                                                                          , T                       origin      = T(0)
                                                                          , ::cblib::math::numerics::RoundingMode  mode        = ::cblib::math::numerics::DEF_DEFAULT_ROUNDING_MODE ) noexcept
{
    x = quantize_decimals(x, digits, origin, mode);
}



//
// *************************************************************************** //
// *************************************************************************** //   END [ X.3.  "quantize" ].







//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ X.  "PUBLIC API" ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib::math".







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_INTERNAL_H  //
