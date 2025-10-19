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
//      1.      PRIMATIVES FOR BÉZIER---MODULE...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//              1A.     ABSTRACTIONS FOR BÉZIER MODULE.
// *************************************************************************** //





// *************************************************************************** //
//              1B.     MINIMAL |R^2 HELPERS.
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
//              1C.     BASIC PRIMATIVES.
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
//              1D.     MORE PRIMATIVES.
// *************************************************************************** //

//  "clamp01"
//      --- utility: clamp to [0,1] with a hair of tolerance ---
//
//  template <class T>
//  inline T clamp01(T v) noexcept { return v < T(0) ? T(0) : (v > T(1) ? T(1) : v); }
//
template <typename T>
inline T clamp01(T t) noexcept
{
    if (t < T(0)) return T(0);
    if (t > T(1)) return T(1);
    return t;
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
    const T         D       = b * b; // std::pow(b, T(0)) - ( T(4) * a * c );
    
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "PRIMATIVES" ]].











// *************************************************************************** //
//
//
//
//      2.      QUADRATIC BÉZIER-CURVE COMPUTATIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//              2A.     PRIMATIVES FOR QUADRATIC BEZIER.
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


//  "eval_quadratic_coord"
//      scalar quadratic coord eval
//
template <typename T>
[[nodiscard]] inline T eval_quadratic_coord(T a0, T c, T b, T t) noexcept {
    const T u = T(1) - t;
    return u*u*a0 + T(2)*u*t*c + t*t*b;
}


//  "eval_quadratic_1d"
//
template <class T>
inline T eval_quadratic_1d(T a, T c, T b, T t) noexcept {
    const T s = T(1) - t;
    return s*s*a + T(2)*s*t*c + t*t*b;
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
//              2B.     SECONDARY FOR QUADRATIC BEZIER.
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
//              2C.     PIXEL-SPACE HELPERS FOR QUADRATIC BEZIER.
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



// *************************************************************************** //
//              2D.     BOUNDING BOX FOR QUADRATIC BEZIER.
// *************************************************************************** //

//  "extremum_param_quadratic_axis"
//      Axis-only param of interior extremum for a quadratic axis.
//      Returns true and writes t∈(0,1) when an interior extremum exists.
//
template <typename T>
[[nodiscard]] inline bool extremum_param_quadratic_axis(T a0, T a1, T a2,
                                                        T& t,
                                                        T eps = static_cast<T>(1e-8)) noexcept
{
    const T denom = a0 - T(2)*a1 + a2;            // from dQ/dt = 0  ⇒  (a0-2a1+a2)·t = (a0-a1)
    if (std::abs(denom) <= eps) return false;     // axis is effectively linear
    const T tt = (a0 - a1) / denom;
    if (tt <= T(0) || tt >= T(1)) return false;   // not interior
    t = tt;
    return true;
}

//  "bbox_union_scalar"
//      Update scalar min/max with a sample value.
//
template <typename T>
inline void bbox_union_scalar(T v, T& lo, T& hi) noexcept {
    if (v < lo) lo = v;
    if (v > hi) hi = v;
}


//  "bbox_quadratic_tight"
//      tight bbox for a quadratic (axis-isolated, aggregating form)
//      Scalar quadratic evaluation for one coordinate
//
//      Exact axis-wise bbox for a quadratic Bézier p(t) = (1−t)^2 p0 + 2(1−t)t p1 + t^2 p2
//      Returns {min, max}. V2 must have .x/.y members; T is a floating scalar type.
//
template <typename V2, typename T = decltype(V2{}.x)>
[[nodiscard]] inline std::pair<V2,V2>
bbox_quadratic_tight(const V2& A, const V2& C, const V2& B,
                     T eps = static_cast<T>(1e-8)) noexcept
{
    // Start with endpoints
    T xmin = (A.x < B.x) ? A.x : B.x;
    T xmax = (A.x > B.x) ? A.x : B.x;
    T ymin = (A.y < B.y) ? A.y : B.y;
    T ymax = (A.y > B.y) ? A.y : B.y;

    // X axis interior extremum
    {
        T tx{};
        if (extremum_param_quadratic_axis(static_cast<T>(A.x),
                                          static_cast<T>(C.x),
                                          static_cast<T>(B.x),
                                          tx, eps))
        {
            const T qx = eval_quadratic_coord(static_cast<T>(A.x),
                                              static_cast<T>(C.x),
                                              static_cast<T>(B.x), tx);
            bbox_union_scalar(qx, xmin, xmax);
        }
    }
    // Y axis interior extremum
    {
        T ty{};
        if (extremum_param_quadratic_axis(static_cast<T>(A.y),
                                          static_cast<T>(C.y),
                                          static_cast<T>(B.y),
                                          ty, eps))
        {
            const T qy = eval_quadratic_coord(static_cast<T>(A.y),
                                              static_cast<T>(C.y),
                                              static_cast<T>(B.y), ty);
            bbox_union_scalar(qy, ymin, ymax);
        }
    }

    return { V2{ xmin, ymin }, V2{ xmax, ymax } };
}


//  "bbox_quadratic_hull"
//      Convenience overload: your quadratic is authored as A (anchor), C_offset (A→control), B (next anchor).
//
template <class V2, class T = decltype(V2{}.x)>
inline std::pair<V2,V2> bbox_quadratic_hull(const V2& A,
                                                   const V2& C_offset,
                                                   const V2& B,
                                                   T eps = T(1e-12)) noexcept
{
    V2 C{ A.x + C_offset.x, A.y + C_offset.y };
    return bbox_quadratic_tight<V2,T>(A, C, B, eps);
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "QUADRATIC" ]].












// *************************************************************************** //
//
//      3.      CUBIC BÉZIER-CURVE COMPUTATIONS...
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



// *************************************************************************** //
//              3A.     PRIMATIVES FOR CUBIC BEZIER.
// *************************************************************************** //

//  "eval_cubic"
//
template <typename V2, typename T>
[[nodiscard]] inline V2 eval_cubic(const V2& A, const V2& C1,
                                   const V2& C2, const V2& B, T t) noexcept
{
    const T u  = T(1) - t;
    const T u2 = u*u,  u3 = u2*u;
    const T t2 = t*t,  t3 = t2*t;
    // (1-t)^3 A + 3(1-t)^2 t C1 + 3(1-t) t^2 C2 + t^3 B
    return v2_add<V2>( v2_add<V2>( v2_mul<V2,T>(A , u3),
                                   v2_mul<V2,T>(C1, T(3)*u2*t) ),
                       v2_add<V2>( v2_mul<V2,T>(C2, T(3)*u*t2),
                                   v2_mul<V2,T>(B , t3) ) );
}

//  "eval_cubic_coord"
//      Scalar cubic evaluation for one coordinate
//
template <typename T>
[[nodiscard]] inline T eval_cubic_coord(T a0, T a1, T a2, T a3, T t) noexcept {
    const T u  = T(1) - t;
    const T u2 = u * u,  u3 = u2 * u;
    const T t2 = t * t,  t3 = t2 * t;
    return u3*a0 + T(3)*u2*t*a1 + T(3)*u*t2*a2 + t3*a3;
}


//  "deriv_cubic"
//
template <typename V2, typename T>
[[nodiscard]] inline V2 deriv_cubic(const V2& A, const V2& C1,
                                    const V2& C2, const V2& B, T t) noexcept {
    const T u  = T(1) - t;
    const T u2 = u*u;
    // 3[(1-t)^2(C1-A) + 2(1-t)t(C2-C1) + t^2(B-C2)]
    const V2 d1 = v2_sub<V2>(C1, A);
    const V2 d2 = v2_sub<V2>(C2, C1);
    const V2 d3 = v2_sub<V2>(B , C2);
    return v2_mul<V2,T>(
        v2_add<V2>( v2_mul<V2,T>(d1, u2),
                    v2_add<V2>( v2_mul<V2,T>(d2, T(2)*u*t),
                                v2_mul<V2,T>(d3, t*t) ) ),
        T(3)
    );
}



// *************************************************************************** //
//              3B.     SECONDARY FOR CUBIC BÉZIER.
// *************************************************************************** //

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


//  "cubic_deriv_quad_coeffs"
//      Given A,C1,C2,B, return qa, qb, qc so that for each axis:
//          (Q'_3/3)(t) = qa * t^2 + qb * t + qc
//
template <typename V2, typename T = decltype(V2{}.x)>
inline void cubic_deriv_quad_coeffs(const V2& A,  const V2& C1,
                                    const V2& C2, const V2& B,
                                    V2& qa, V2& qb, V2& qc) noexcept
{
    //      1.      a = -A + 3C1 - 3C2 + B
    qa.x = -A.x + T(3)*C1.x - T(3)*C2.x + B.x;
    qa.y = -A.y + T(3)*C1.y - T(3)*C2.y + B.y;
    
    //      2.      b = 2*(A - 2C1 + C2)
    qb.x = T(2)*(A.x - T(2)*C1.x + C2.x);
    qb.y = T(2)*(A.y - T(2)*C1.y + C2.y);
    
    //      3.      c = -A + C1
    qc.x = -A.x + C1.x;
    qc.y = -A.y + C1.y;
    return;
}


//  "cubic_extrema_params_axis"
//      --- auxiliary: extrema params for one axis via quadratic solver --------------
//      Finds t in (0,1) solving qa*t^2 + qb*t + qc = 0.
//      Uses your existing quadratic root semantics (eps guards & dedup).
//
template <typename T>
inline void cubic_extrema_params_axis(T qa, T qb, T qc,
                                      T out[2], int& count,
                                      T eps = static_cast<T>(1e-8)) noexcept
{
    //  Reuse the same endpoint/epsilon policy as quadratic_unit_roots
    quadratic_unit_roots<T>(qa, qb, qc, out, count, eps);
    
    //  Clamp defensively (the helper already filters, this is idempotent)
    for (int i = 0; i < count; ++i)
        { out[i] = clamp01(out[i]); }
    
    return;
}





// *************************************************************************** //
//              3C.     PIXEL-SPACE HELPERS FOR CUBIC BEZIER.
// *************************************************************************** //

namespace px { //     BEGINNING NAMESPACE "px"...
// *************************************************************************** //
// *************************************************************************** //

    //
    //      ...
    //

// *************************************************************************** //
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "px" NAMESPACE.



// *************************************************************************** //
//              3D.     BOUNDING-BOX FOR CUBIC BÉZIER.
// *************************************************************************** //

//  "bbox_cubic_hull"
//      --- conservative control-hull AABB (min/max of control cage) -----------------
//
template <typename V2>
inline void bbox_cubic_hull(const V2& A, const V2& C1,
                            const V2& C2, const V2& B,
                            V2& tl, V2& br, bool& first) noexcept {
    const V2 pts[4] = {A, C1, C2, B};
    for (const V2& p : pts) {
        if (first) { tl = br = p; first = false; }
        else {
            tl.x = (p.x < tl.x) ? p.x : tl.x;  tl.y = (p.y < tl.y) ? p.y : tl.y;
            br.x = (p.x > br.x) ? p.x : br.x;  br.y = (p.y > br.y) ? p.y : br.y;
        }
    }
}


//  "bbox_cubic_tight"
//      --- tight analytic AABB (endpoints + axis extrema) ---------------------------
//      Matches the shader: include endpoints, then add Q_3(t*) for all interior roots
//      of dQx/dt and dQy/dt that lie in (0,1).
//
template <typename V2, typename T = decltype(V2{}.x)>
inline void bbox_cubic_tight(const V2& A, const V2& C1,
                             const V2& C2, const V2& B,
                             V2& tl, V2& br, bool& first,
                             T eps = static_cast<T>(1e-8)) noexcept
{
    auto add_pt = [&](T x, T y) noexcept {
        if (first) { tl.x = br.x = x; tl.y = br.y = y; first = false; return; }
        tl.x = (x < tl.x) ? x : tl.x;  tl.y = (y < tl.y) ? y : tl.y;
        br.x = (x > br.x) ? x : br.x;  br.y = (y > br.y) ? y : br.y;
    };

    // Always include endpoints
    add_pt(A.x, A.y);
    add_pt(B.x, B.y);

    // Helper: process one axis independently (component-wise shader logic)
    auto process_axis = [&](T a0, T a1, T a2, T a3, T& minv, T& maxv) noexcept {
        const T k0 = -a0 + a1;
        const T k1 =  a0 - T(2)*a1 + a2;
        const T k2 = -a0 + T(3)*a1 - T(3)*a2 + a3;

        const T h  = k1*k1 - k0*k2;                // discriminant of derivative quad

        if (h > eps) {
            const T root = std::sqrt(h);

            auto try_root = [&](T denom) noexcept {
                if (std::abs(denom) <= eps) return;            // guard
                T t = k0 / denom;                               // stable form (shader)
                if (t <= T(0) || t >= T(1)) return;             // only interior roots
                const T q = eval_cubic_coord(a0, a1, a2, a3, t);
                if (q < minv) minv = q;
                if (q > maxv) maxv = q;
            };

            // Two candidate roots for this axis
            try_root(-k1 - root);
            try_root(-k1 + root);
        }
    };

    // Initialize axis min/max with current box edges
    T xmin = tl.x, xmax = br.x;
    T ymin = tl.y, ymax = br.y;

    // Axis-isolated extrema updates (NO cross-coupling!)
    process_axis(static_cast<T>(A.x), static_cast<T>(C1.x),
                 static_cast<T>(C2.x), static_cast<T>(B.x),
                 xmin, xmax);

    process_axis(static_cast<T>(A.y), static_cast<T>(C1.y),
                 static_cast<T>(C2.y), static_cast<T>(B.y),
                 ymin, ymax);

    // Write back tightened edges
    tl.x = xmin; br.x = xmax;
    tl.y = ymin; br.y = ymax;
}
//
/*
    // 1) Always include endpoints
    if (first) { tl = br = A; first = false; }
    else {
        tl.x = (A.x < tl.x) ? A.x : tl.x;  tl.y = (A.y < tl.y) ? A.y : tl.y;
        br.x = (A.x > br.x) ? A.x : br.x;  br.y = (A.y > br.y) ? A.y : br.y;
    }
    tl.x = (B.x < tl.x) ? B.x : tl.x;  tl.y = (B.y < tl.y) ? B.y : tl.y;
    br.x = (B.x > br.x) ? B.x : br.x;  br.y = (B.y > br.y) ? B.y : br.y;

    // 2) Axis extrema from derivative quadratic coeffs
    V2 qa{}, qb{}, qc{};
    cubic_deriv_quad_coeffs<V2,T>(A, C1, C2, B, qa, qb, qc);

    // X axis roots
    {
        T tx[2]; int nx = 0;
        cubic_extrema_params_axis<T>(qa.x, qb.x, qc.x, tx, nx, eps);
        for (int i = 0; i < nx; ++i) {
            const V2 q = eval_cubic<V2,T>(A, C1, C2, B, tx[i]);
            // only X is guaranteed extremal; still safe to update both axes with q
            tl.x = (q.x < tl.x) ? q.x : tl.x;
            br.x = (q.x > br.x) ? q.x : br.x;
            // Y update is harmless and keeps bbox conservative/tight
            tl.y = (q.y < tl.y) ? q.y : tl.y;
            br.y = (q.y > br.y) ? q.y : br.y;
        }
    }
    // Y axis roots
    {
        T ty[2]; int ny = 0;
        cubic_extrema_params_axis<T>(qa.y, qb.y, qc.y, ty, ny, eps);
        for (int i = 0; i < ny; ++i) {
            const V2 q = eval_cubic<V2,T>(A, C1, C2, B, ty[i]);
            tl.x = (q.x < tl.x) ? q.x : tl.x;
            br.x = (q.x > br.x) ? q.x : br.x;
            tl.y = (q.y < tl.y) ? q.y : tl.y;
            br.y = (q.y > br.y) ? q.y : br.y;
        }
    }
}*/






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "CUBIC" ]].








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
