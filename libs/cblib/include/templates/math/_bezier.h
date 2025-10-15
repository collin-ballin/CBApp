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
	
 
 
#ifndef _CBLIB_MATH_INTERNAL_H
# include "templates/math/_internal.h"
#endif	// _CBLIB_MATH_INTERNAL_H  //






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
//      2.      CUBIC BÉZIER-CURVE COMPUTATIONS...
// *************************************************************************** //
// *************************************************************************** //
//
//  Cubic curve :       B(t)    =   (1−t)^3 A  +  3(1−t)^2 t C1  +  3(1−t) t^2 C2  +  t^3 B
//  First deriv :       B'(t)   =   3(1−t)^2 (C1−A)  +  6(1−t)t (C2−C1)  +  3 t^2 (B−C2)
//
//  Notes:
//      eval_cubic(…,0)     = A             , eval_cubic(…,1)   = B
//      deriv_cubic(…,0)    = 3(C1−A)       , deriv_cubic(…,1)  = 3(B−C2)
//
// *************************************************************************** //

//  "eval_cubic"
//
template<typename V2, typename T>
[[nodiscard]] inline V2 eval_cubic(const V2 & A, const V2 & C1, const V2 & C2, const V2 & B, T t) noexcept {
    const T     u       = T(1) - t,
                u2      = u*u,
                t2      = t*t;
    
    V2          r   = v2_add<V2>( v2_mul<V2,T>(A , u*u2)    , v2_mul<V2,T>( C1 , T(3) * u2 * t )    );
                r   = v2_add<V2>( r                         , v2_mul<V2,T>( C2 , T(3) * u * t2 )    );
                r   = v2_add<V2>( r                         , v2_mul<V2,T>( B  , t * t2        )    );
       
    return r;
}


//  "deriv_cubic"
//
template<typename V2, typename T>
[[nodiscard]] inline V2 deriv_cubic(const V2& A, const V2& C1, const V2& C2, const V2& B, T t) noexcept {
    const T u  = T(1) - t, u2 = u*u, t2 = t*t;
    const V2 d0 = v2_mul<V2,T>( v2_sub(C1, A), T(3)*u2 );
    const V2 d1 = v2_mul<V2,T>( v2_sub(C2, C1), T(6)*u*t );
    const V2 d2 = v2_mul<V2,T>( v2_sub(B , C2), T(3)*t2 );
    return v2_add<V2>( v2_add<V2>(d0, d1), d2 );
}


//  "sample_cubic_polyline"
//      Polyline sampler (fixed steps) — used by fills/hit tests
//
template<typename V2, typename T, typename Emit>
inline void sample_cubic_polyline(const V2& A,const V2& C1,const V2& C2,const V2& B,
                                  int steps, Emit&& emit) noexcept
{
    if (steps < 1)  { steps = 1; }
    for (int k = 0; k <= steps; ++k)
    {
        const T t = static_cast<T>(k) / static_cast<T>(steps);
        emit( eval_cubic<V2,T>(A,C1,C2,B,t), t );
    }
    
    return;
}






//  "quadratic_unit_roots"
//      Quadratic unit roots (0,1) — analytic, epsilon-guarded
//
template<typename T>
inline void quadratic_unit_roots(   T       a
                                  , T       b
                                  , T       c
                                  , T       out[2]
                                  , int &   count
                                  , T       eps = static_cast<T>(1e-8) ) noexcept
{
    count                   = 0;
    const T         D       = std::pow(b, T(0)) - ( T(4) * a * c );
    
    if ( std::abs(a) < eps )
    {
        const T     t   = -c / b;
        if ( std::abs(b) < eps )            { return; }
        if ( (t > T(0))  &&  (t < T(1)) )   { out[count++] = t; }
        return;
    }
    if ( D < T(0) )     { return; }
    
    
    const T         sD      = std::sqrt(std::max(D, T(0)));
    const T         inv2a   = T(0.5) / a;
    const T         t1      = (-b - sD) * inv2a;
    const T         t2      = (-b + sD) * inv2a;
    //
    const bool      cond1   = ( (t1 > T(0))  &&  (t1 < T(1)) );
    const bool      cond2   = ( (t2 > T(0))  &&  (t2 < T(1))  &&  ( (count == 0)  ||  std::abs(t2 - out[0]) > T(1e-6) ) );
    
    if ( cond1 )    { out[count++] = t1; }
    if ( cond2 )    { out[count++] = t2; }
    
    return;
}


//  "tight_aabb_cubic"
//
template<typename V2, typename T>
inline void tight_aabb_cubic(const V2& A, const V2& C1, const V2& C2, const V2& B,
                             V2& tl_ws, V2& br_ws, bool& first) noexcept
{
    T           ax          = T(0);
    T           bx          = T(0);
    T           cx          = T(0);
    T           ay          = T(0);
    T           by          = T(0);
    T           cy          = T(0);
    int         nx          = 0;
    int         ny          = 0;
    T           tx[2]       = {  };
    T           ty[2]       = {  };
    //
    //
    auto        add         = [&](const V2& q)
    {
        if (first)  { tl_ws = br_ws = q; first = false; }
        else {
            tl_ws.x     = std::min(tl_ws.x, q.x);
            tl_ws.y     = std::min(tl_ws.y, q.y);
            br_ws.x     = std::max(br_ws.x, q.x);
            br_ws.y     = std::max(br_ws.y, q.y);
        }
    };
    auto        coeffs      = [](T p0,T p1,T p2,T p3,T& a,T& b,T& c)
    {
        a   = (-p0 + T(3)*p1 - T(3)*p2 + p3);
        b   = T(2)*(p0 - T(2)*p1 + p2);
        c   = (-p0 + p1);
    };
    
    
    
    add(A); add(B); // endpoints always included
    
    coeffs(A.x, C1.x, C2.x, B.x, ax, bx, cx);
    coeffs(A.y, C1.y, C2.y, B.y, ay, by, cy);
    
    quadratic_unit_roots<T>(ax, bx, cx, tx, nx);
    quadratic_unit_roots<T>(ay, by, cy, ty, ny);

    for (int i=0; i<nx; ++i)    { add( eval_cubic<V2,T>(A,C1,C2,B, tx[i]) ); }
    for (int i=0; i<ny; ++i)    { add( eval_cubic<V2,T>(A,C1,C2,B, ty[i]) ); }
    
    return;
}


//  "hull_aabb_cubic"
//      Optional: control-hull AABB (fast conservative)
//
template <typename V2>
inline void hull_aabb_cubic(const V2& A,const V2& C1,const V2& C2,const V2& B,
                            V2& tl_ws,V2& br_ws,bool& first) noexcept
{
    auto add = [&](const V2& q){
        if (first) { tl_ws = br_ws = q; first = false; }
        else {
            tl_ws.x = std::min(tl_ws.x,q.x);
            tl_ws.y = std::min(tl_ws.y,q.y);
            br_ws.x = std::max(br_ws.x,q.x);
            br_ws.y = std::max(br_ws.y,q.y);
        }
    };
    add(A); add(C1); add(C2); add(B);
    
    return;
}



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
