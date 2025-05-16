/***********************************************************************************
*
*       ********************************************************************
*       ****                 M A T H . H  ____  F I L E                 ****
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
#ifndef _CBLIB_MATH_H
#define _CBLIB_MATH_H 1

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



namespace cblib {   //     BEGINNING NAMESPACE "cblib"...
// *************************************************************************** //
// *************************************************************************** //



//  1.  GENERAL MATH STUFF...
// *************************************************************************** //
// *************************************************************************** //



//  1.  MATH STUFF...
// *************************************************************************** //
// *************************************************************************** //

/*
// constexpr power-of-10 helper
template<typename T, int N>
constexpr T pow10_helper() {
    if constexpr (N > 0)  return T(10) * pow10_helper<T, N - 1>();
    else if constexpr (N < 0) return T(1) / pow10_helper<T, -N>();
    else                    return T(1);
}

// round_to: always rounds up (ceiling) to N decimal places
// Enabled only for floating-point types via SFINAE
template< typename T, int N,
          typename = std::enable_if_t<std::is_floating_point_v<T>> >
inline T round_to(T value)
{
    constexpr T base = pow10_helper<T, N>();
    return std::ceil(value * base) / base;
}
*/




//  "pow10_IMPL"
//      - Constexpr power-of-10 helper.
template<typename T, auto N>
constexpr T pow10_IMPL(void) {
    if constexpr (N > 0)  return T(10) * pow10_helper<T, N - 1>();
    else if constexpr (N < 0) return T(1) / pow10_helper<T, -N>();
    else                    return T(1);
}

//  "round_to"
//
//      - Always rounds up (ceiling) to N decimal places.
//      - Enabled only for floating-point types via SFINAE.
template<auto N, typename T>
inline T round_to(const T value) {
    T base = std::pow( T(10), N );
    return std::ceil(value * base) / base;
}





//  "is_close"
//  checks if two floating-point values are close within
//  an absolute tolerance and/or a relative tolerance.
//  T must be a floating-point type; abs_tol and rel_tol are non-type template params
//  Optimized via if constexpr to strip unused branches, constexpr for compile-time use,
//  noexcept and [[nodiscard]] for clarity.
//
/*
template<
#if __cpp_concepts >= 201907L
    std::floating_point T,
#else
    typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>,
#endif
    T AbsTol = static_cast<T>(1e-9),
    T RelTol = static_cast<T>(0)
>
[[nodiscard]]
inline constexpr bool is_close(T a, T b) noexcept {
    static_assert(AbsTol >= static_cast<T>(0), "abs_tol must be non-negative");
    static_assert(RelTol >= static_cast<T>(0), "rel_tol must be non-negative");

    T diff = std::fabs(a - b);
    if constexpr (RelTol == static_cast<T>(0))      // only absolute tolerance
    { return diff <= AbsTol; }
    T max_ab = std::max(std::fabs(a), std::fabs(b));
    T max_diff = std::max(RelTol * max_ab, AbsTol);
    return diff <= max_diff;
}
*/




// *************************************************************************** //
//
//
//
//  2.  HELPFUL MATH OBJECTS...
// *************************************************************************** //
// *************************************************************************** //












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cblib" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_H  //
