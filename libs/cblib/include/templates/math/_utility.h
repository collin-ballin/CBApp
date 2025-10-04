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
#include <algorithm>            //  std::max
#if __cpp_concepts >= 201907L
# include <concepts>            //  std::floating_point
#endif  //  C++20.  //

#include <cstdint>
#include <cassert>

#include <cmath>                //  std::abs, std::isnan, std::isinf, std::fpclassify, FP_ZERO
#include <cstddef>
#include <iomanip>


#include <vector>
#include <array>
#include <stdexcept>            //  std::invalid_argument
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	//  C++11.  //
	
 
 
#ifndef _CBLIB_MATH_INTERNAL_H
# include "templates/math/_internal.h"
#endif	// _CBLIB_MATH_INTERNAL_H  //



namespace cblib { namespace math {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      GENERIC / GENERAL MATH STUFF...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1.1     "round_to" Function.
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
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.1.  round_to".






// *************************************************************************** //
//
//
//
//      1.2.    "is_close" Function.
// *************************************************************************** //
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
/// @see                cblib::default_rel_tol_v
//
template <std::floating_point T>
[[nodiscard]] inline constexpr bool is_close(   const T     a
                                              , const T     b
                                              , T           rel_tol = ::cblib::math::numerics::default_rel_tol<T>()
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
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.2.  is_close".






// *************************************************************************** //
//
//
//
//      1.3.    "quantize" Function.
// *************************************************************************** //
// *************************************************************************** //

//  "quantize"      [ CORE-IMPLEMENTATION ].
//
//      Quantize  x  onto the lattice,  { origin + k * quantum,  where k | k ∈ ℤ }.
//      Preconditions :     quantum > 0 (returns x unchanged if violated).
//                          NaN / ±inf pass through unchanged.
//
template <std::floating_point T>
[[nodiscard]] inline T                      quantize_IMPL           (T x, T quanta, T origin, numerics::RoundingMode mode) noexcept
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


//  "quantize"      [ WRAPPER-IMPLEMENTATIONS ].
//
template <std::floating_point T, std::floating_point U>
[[nodiscard]] inline T                      quantize                (   T                       x
                                                                      , U                       quanta
                                                                      , U                       origin  = U(0)
                                                                      , numerics::RoundingMode  mode    = numerics::DEF_DEFAULT_ROUNDING_MODE ) noexcept
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
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.3.  quantize".






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MATH UTILITIES".












// *************************************************************************** //
//
//
//
//      X.      SIMPLE COMPILE-TIME UNIT TESTS...
// *************************************************************************** //
// *************************************************************************** //
namespace numerics_tests {//     BEGINNING NAMESPACE "numerics_tests"...



// *************************************************************************** //
//              X.0.    UTILITIES.
// *************************************************************************** //

#define     __NEVER_DEFINE_THIS__           1
using       namespace                       ::cblib::math::numerics;
using       Mode                            = RoundingMode;



// *************************************************************************** //
//              X.1.    "ROUND_TO" TESTS.
// *************************************************************************** //

//
// *************************************************************************** //
// *************************************************************************** //   END "ROUND_TO TESTS".






// *************************************************************************** //
//              X.2.    "IS_CLOSE" TESTS.
// *************************************************************************** //

//  BATCH #1.   ALWAYS CONSTEXPR-SAFE, FINITE TESTS...
/*
static_assert(  is_close    (1.0        , 1.0                                       )   );          //  exact equality (finite)
static_assert(  is_close    (+0.0       , -0.0                                      )   );          //  signed zeros considered equal
static_assert(  is_close    (1000.0     , 1000.0000005      , 1e-9                  )   );          //  relative tol pass
static_assert(  !is_close   (1000.0     , 1000.002          , 1e-9                  )   );          //  relative tol fail
static_assert(  is_close    (1e-12      , 0.0               , 1e-9      , 1e-9      )   );          //  near zero requires abs_tol

//              1.1.    symmetry.
static_assert(  is_close    (1.2345     , 1.2345000001, 1e-9) == is_close(1.2345000001, 1.2345, 1e-9)    );

//              1.2.    invalid tolerances in constant evaluation → false (by design)
static_assert(  !is_close(1.0, 1.0, -1e-9)                                                      );  //  rel_tol < 0
static_assert(  !is_close(1.0, 1.0, 1e-9, -1e-12)                                               );  //  abs_tol < 0

//              1.3.    type coverage
static_assert(  is_close<float>(1.0f, 1.0f)                                                     );
static_assert(  is_close<long double>((long double)2.0, (long double)2.0)                       );



//  BATCH #2A.  TESTS THAT REQUIRE "CONSTEXPR <CMATH> [C++23]"...
#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L
//
//
    constexpr double INF = std::numeric_limits<double>::infinity();
    constexpr double NNF = -std::numeric_limits<double>::infinity();
    constexpr double QN  = std::numeric_limits<double>::quiet_NaN();

    //  equal infinities (same sign) → true
    static_assert(is_close(INF, INF));
    static_assert(is_close(NNF, NNF));

    //  unequal infinities or finite vs inf → false
    static_assert(!is_close(INF, NNF));
    static_assert(!is_close(INF, 1e308));
    static_assert(!is_close(-INF, -1e308));

    //  any NaN → false
    static_assert(!is_close(QN, 0.0));
    static_assert(!is_close(0.0, QN));
    static_assert(!is_close(QN, QN));
//
//
#endif  //  defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L  //
*/



//
// *************************************************************************** //
// *************************************************************************** //   END "IS_CLOSE TESTS".






// *************************************************************************** //
//              X.3.    "QUANTIZE" TESTS.
// *************************************************************************** //

//  BATCH #1.   CONCEPTS / SUBSTITUTION CHECKS...
//
//              1.1.    quantize: only floating-point T accepted
static_assert( requires(double x)       { quantize(x, 1.0   );      }       );
static_assert( requires(float  x)       { quantize(x, 0.25f );      }       );
static_assert( requires(long double x)  { quantize(x, 0.1L  );      }       );

//              1.2.    deduces T=int if not guided — rejected by constraint    [ THIS SHOULD *NOT* COMPILE ].
#ifndef __NEVER_DEFINE_THIS__
    static_assert( !requires(int x)         { quantize(x, 1.0   );      }       );
    static_assert( !requires(double x)      { quantize(x, 1     );      }       );
#endif //  __NEVER_DEFINE_THIS__  //

//              1.3.    quantize with origin + mode
static_assert( requires(double x) {
    quantize(x, 0.5, 0.0, Mode::ties_to_even);
});

//              1.4.    in-place overload exists and is callable
static_assert( requires(double & x)     { quantize_inplace(x, 0.5, 0.0, Mode::toward_neg_inf);  }   );

//              1.5.    decimals interface exists
static_assert( requires(double x)       { quantize_decimals(x, 3, 0.0, Mode::toward_pos_inf);   }   );
static_assert( requires(float& x)       { quantize_decimals_inplace(x, -2);                     }   );



//  BATCH #2.   RETURN TYPES  AND  NOEXCEPT PROPERTIES...
//
//              2.1.    return types are same as input T
static_assert( std::same_as<decltype( quantize(          std::declval< double       >(),    0.5     )   ), double       >   );
static_assert( std::same_as<decltype( quantize(          std::declval< float        >(),    0.25f   )   ), float        >   );
static_assert( std::same_as<decltype( quantize_decimals( std::declval< long double  >(),    3       )   ), long double  >   );

//              2.2.    in-place overloads return void
static_assert( std::same_as<decltype( quantize_inplace(             std::declval< double& >(), 1.0  )   ), void>    );
static_assert( std::same_as<decltype( quantize_decimals_inplace(    std::declval< float&  >(), 2    )   ), void>    );

//              2.3.    noexcept guarantees
static_assert( noexcept( quantize(                  std::declval< double  >(),  1.0,    0.0, Mode::ties_to_even     )  )   );
static_assert( noexcept( quantize_inplace(          std::declval< double& >(),  1.0,    0.0, Mode::ties_to_even     )  )   );
static_assert( noexcept( quantize_decimals(         std::declval< double  >(),  3,      0.0, Mode::ties_to_even     )  )   );
static_assert( noexcept( quantize_decimals_inplace( std::declval< double& >(),  3,      0.0, Mode::ties_to_even     )  )   );



//  BATCH #3.   OVERLOAD DISAMBIGUATION  AND  DEFAULTS...
//
//              3.1.    defaulted origin + mode are available
static_assert( requires(double x)   { quantize(x, 0.25);        }   );
static_assert( requires(double x)   { quantize_decimals(x, 4);  }   );

//              3.2.    RoundingMode enum is usable in unevaluated context
static_assert( static_cast<int>(Mode::ties_to_even)    != static_cast<int>(Mode::toward_zero)         );
static_assert( static_cast<int>(Mode::toward_pos_inf)  != static_cast<int>(Mode::toward_neg_inf)      );
static_assert( static_cast<int>(Mode::away_from_zero)  >= 0                                         );


//  BATCH #4.   SFINAE SANITY FOR NON-FLOATING-POINT TYPES  [ THIS SHOULD *NOT* COMPILE ]...
#ifndef __NEVER_DEFINE_THIS__
    struct NotFloat {};
    static_assert( !requires(NotFloat v)    { quantize             (v,     1.0     );  }   );
    static_assert( !requires(NotFloat v)    { quantize_decimals    (v,     2       );  }   );
#endif //  __NEVER_DEFINE_THIS__  //


//  BATCH #5.   LONG DOUBLE---FLOAT MIXING (MAKE SURE IT DOES NOT ACCIDENTALLY WIDEN)...
static_assert( std::same_as<decltype(  quantize(std::declval<float>(),          0.1f,   0.0f, Mode::toward_zero    )   ), float>           );
static_assert( std::same_as<decltype(  quantize(std::declval<long double>(),    0.1L,   0.0L, Mode::away_from_zero )   ), long double>     );

//
// *************************************************************************** //
// *************************************************************************** //   END "QUANTIZE TESTS".



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "numerics_tests".






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "X.  COMPILE-TIME TESTS".


















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
