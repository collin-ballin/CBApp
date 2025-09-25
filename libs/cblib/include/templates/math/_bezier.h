/***********************************************************************************
*
*       ********************************************************************
*       ****              _ B E Z I E R . H  ____  F I L E              ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      SEPTEMBER 21, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_MATH_BEZIER_H
#define _CBLIB_MATH_BEZIER_H 1

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

namespace bezier { //     BEGINNING NAMESPACE "bezier"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      QUADRATIC BÉZIER-CURVE COMPUTATIONS...
// *************************************************************************** //
// *************************************************************************** //


// *************************************************************************** //
//              1.1.    MINIMAL |R^2 HELPERS.
// *************************************************************************** //

//  "v2_make"
template <typename V2, typename T>
[[nodiscard]] inline V2 v2_make(T x, T y) noexcept { V2 r; r.x = x; r.y = y; return r; }

//  "v2_add"
template <typename V2>
[[nodiscard]] inline V2 v2_add(const V2& a, const V2& b) noexcept { return v2_make<V2>(a.x + b.x, a.y + b.y); }

//  "v2_sub"
template <typename V2>
[[nodiscard]] inline V2 v2_sub(const V2& a, const V2& b) noexcept { return v2_make<V2>(a.x - b.x, a.y - b.y); }

//  "v2_mul"
template <typename V2, typename T>
[[nodiscard]] inline V2 v2_mul(const V2& a, T s) noexcept { return v2_make<V2>(a.x * s, a.y * s); }

//  "v2_dot"
template <typename V2>
[[nodiscard]] inline auto v2_dot(const V2& a, const V2& b) noexcept { return a.x * b.x + a.y * b.y; }

//  "v2_len"
template <typename V2>
[[nodiscard]] inline auto v2_len(const V2& a) noexcept {
    using T = decltype(a.x + a.y);
    return static_cast<T>(std::sqrt(static_cast<T>(a.x*a.x + a.y*a.y)));
}



// *************************************************************************** //
//              1.2.    BASIC PRIMATIVES.
// *************************************************************************** //

//  "normalize"
//      Return unit vector in the direction of v (or {0,0} if |v| is tiny).
//
template <typename V2, typename T = decltype(V2{}.x)>
[[nodiscard]] inline V2 normalize(const V2& v, T eps = static_cast<T>(1e-12)) noexcept {
    const T L = v2_len(v);
    if (L <= eps || !(L > static_cast<T>(0))) return v2_make<V2>(static_cast<T>(0), static_cast<T>(0));
    const T inv = static_cast<T>(1) / L;
    return v2_mul<V2,T>(v, inv);
}

//  "perp_left"
//      Return a left-hand normal to v (not normalized).
//
template <typename V2, typename T = decltype(V2{}.x)>
[[nodiscard]] inline V2 perp_left(const V2& v) noexcept { return v2_make<V2>(-v.y, v.x); }






// *************************************************************************** //
//              1.3.    PRIMATIVES FOR QUADRATIC BEZIER.
// *************************************************************************** //

//  "eval_quadratic"
//      @brief Evaluate quadratic Bézier Q(t) = (1−t)^2 A + 2(1−t)t C + t^2 B.
//
template <typename V2, typename T>
[[nodiscard]] inline V2 eval_quadratic(const V2& A, const V2& C, const V2& B, T t) noexcept {
    const T u  = static_cast<T>(1) - t;
    const T u2 = u * u;
    const T tt = t * t;
    // u^2*A + 2*u*t*C + t^2*B
    return v2_add<V2>( v2_add<V2>( v2_mul<V2,T>(A, u2), v2_mul<V2,T>(C, static_cast<T>(2)*u*t) ),
                       v2_mul<V2,T>(B, tt) );
}

//  "deriv_quadratic"
//      @brief First derivative Q'(t) = 2(1−t)(C−A) + 2t(B−C).
//
template<typename V2, typename T>
[[nodiscard]] inline V2 deriv_quadratic(const V2& A, const V2& C, const V2& B, T t) noexcept {
    const T u = static_cast<T>(1) - t;
    // 2[(1−t)(C−A) + t(B−C)]
    const V2 CA = v2_sub(C, A);
    const V2 BC = v2_sub(B, C);
    return v2_mul<V2,T>( v2_add<V2>( v2_mul<V2,T>(CA, u), v2_mul<V2,T>(BC, t) ),
                         static_cast<T>(2) );
}

//  "tangent_quadratic_unit"
//      @brief Unit tangent T̂(t) of quadratic Bézier.
//
template<typename V2, typename T>
[[nodiscard]] inline V2 tangent_quadratic_unit(const V2& A, const V2& C, const V2& B, T t,
                                               T eps = static_cast<T>(1e-12)) noexcept {
    const V2 d = deriv_quadratic<V2,T>(A, C, B, t);
    return normalize<V2,T>(d, eps);
}

//  "normal_quadratic_unit"
//      Left-hand unit normal N̂(t) to the quadratic Bézier.
//
template<typename V2, typename T>
[[nodiscard]] inline V2 normal_quadratic_unit(const V2& A, const V2& C, const V2& B, T t,
                                              T eps = static_cast<T>(1e-12)) noexcept {
    const V2 T_hat = tangent_quadratic_unit<V2,T>(A, C, B, t, eps);
    return normalize<V2,T>(perp_left<V2>(T_hat), eps);
}



// *************************************************************************** //
//              1.4.    ...
// *************************************************************************** //

//  "sample_quadratic_polyline"
//      Emit a fixed-step polyline along a quadratic Bézier; calls emit(P, t) for k=0..steps.
//
template<typename V2, typename T, typename Emit>
inline void sample_quadratic_polyline(const V2& A, const V2& C, const V2& B,
                                      int steps, Emit&& emit) noexcept
{
    if (steps < 1) steps = 1;
    // k = 0..steps inclusive: t in [0,1]
    for (int k = 0; k <= steps; ++k) {
        const T t = static_cast<T>(k) / static_cast<T>(steps);
        const V2 P = eval_quadratic<V2,T>(A, C, B, t);
        emit(P, t);
    }
}

//  "sample_quadratic_polyline_push"
//      Append a fixed-step polyline of a quadratic Bézier into a container with push_back(V2).
//
template<typename V2, typename T, typename Container>
inline void sample_quadratic_polyline_push(const V2& A, const V2& C, const V2& B,
                                           int steps, Container& out) noexcept
{
    sample_quadratic_polyline<V2,T>(A, C, B, steps,
        [&](const V2& P, T /*t*/){ out.push_back(P); });
}






// *************************************************************************** //
//              1.5.    PIXEL-SPACE HELPERS.
// *************************************************************************** //

namespace px { //     BEGINNING NAMESPACE "px"...
// *************************************************************************** //
// *************************************************************************** //

//  "dist2"
//      Return squared Euclidean distance between two 2D points (pixels).
//
template <typename V2>
[[nodiscard]] inline float dist2(const V2& a, const V2& b) noexcept {
    const float dx = static_cast<float>(a.x) - static_cast<float>(b.x);
    const float dy = static_cast<float>(a.y) - static_cast<float>(b.y);
    return dx*dx + dy*dy;
}


//  "lerp"
//      Linear interpolation on a segment AB (pixels):  L(u) = (1−u)A + uB.
//
template <typename V2>
[[nodiscard]] inline V2 lerp(const V2& A, const V2& B, float u) noexcept {
    const float v = 1.0f - u;
    return V2{ v*A.x + u*B.x, v*A.y + u*B.y };
}


//  "project_param_on_segment"
//      Project point P onto segment AB (pixels); return clamped parameter  u ∈ [0,1].
//
template <typename V2>
[[nodiscard]] inline float project_param_on_segment(const V2& A, const V2& B, const V2& P) noexcept {
    const float ABx = static_cast<float>(B.x) - static_cast<float>(A.x);
    const float ABy = static_cast<float>(B.y) - static_cast<float>(A.y);
    const float APx = static_cast<float>(P.x) - static_cast<float>(A.x);
    const float APy = static_cast<float>(P.y) - static_cast<float>(A.y);
    const float len2 = ABx*ABx + ABy*ABy;
    
    if ( len2 <= 0.0f || !(len2 > 0.0f) )       { return 0.0f; }          // degenerate : stick to A
    float u = (APx*ABx + APy*ABy) / len2;
    
    if (u < 0.0f)       { u = 0.0f; }
    else if (u > 1.0f)  { u = 1.0f; }       // clamp
    
    return u;
}


//  "is_endpoint_param"
//      True if parameter u is effectively at an endpoint (within eps), using is_close().
//
template <typename T>
[[nodiscard]] inline bool is_endpoint_param(T u, T eps = static_cast<T>(1e-6))
{
    return is_close<T>(u, static_cast<T>(0), static_cast<T>(0), eps) ||
           is_close<T>(u, static_cast<T>(1), static_cast<T>(0), eps);
}



// *************************************************************************** //
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "px" NAMESPACE.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "QUADRATIC" BÉZIER.






// *************************************************************************** //
//
//
//
//      2.      CUBIC BÉZIER-CURVE COMPUTATIONS...
// *************************************************************************** //
// *************************************************************************** //

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CUBIC" BÉZIER.












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "bezier" NAMESPACE.



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "math" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_BEZIER_H  //
