/***********************************************************************************
*
*       ********************************************************************
*       ****             _ U T I L I T Y . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 12, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_IMGUI_EXTENSIONS_UTILITY_H
#define _CBLIB_IMGUI_EXTENSIONS_UTILITY_H 1
#include CBAPP_USER_CONFIG

//      C++ Standard Libraries.
#include <iostream>
#include <stdexcept>    // std::runtime_error
#include <vector>
#include <deque>
//
#include <type_traits>
#include <functional>  // std::hash
#include <concepts>    // std::integral
#include <algorithm>
//
#include <compare>     // <=>


//      C-Libraries.
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <cstddef>     // size_t
#include <cstdint>     // uint32_t, uint16_t, …
#include <cassert>     // assert …


//      **My** Headers.
#include "json.hpp"
#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers



namespace cblib { namespace utl {   //     BEGINNING NAMESPACE "cblib" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      CONSTEXPR COLOR UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//              1.1.    UTILITY FUNCTIONS.
// *************************************************************************** //

//  "to_u8"
[[nodiscard]] inline constexpr ImU32 to_u8(float f) {
    return static_cast<ImU32>(
                (f <= 0.0f)
                    ? 0u
                    : (f >= 1.0f)
                        ? 255u
                        : f * 255.0f + 0.5f);          // round-to-nearest
}


//  "_absf_"
//      constexpr helpers (local, minimal)
//
inline constexpr float _absf_(float x) noexcept { return x >= 0.0f ? x : -x; }


//  "_wrap01_"
//      Wrap a float into [0,1). Matches fmod(x,1) for typical hue ranges.
//      Note: relies on truncation; very large |x| could overflow the int cast.
//
inline constexpr float _wrap01_(float x) noexcept
{
    const int   n = static_cast<int>(x);          // trunc toward 0
    const float f = x - static_cast<float>(n);
    return (f < 0.0f) ? (f + 1.0f) : f;
}






// *************************************************************************** //
//              1.2.    MAIN COLOR CONVERTERS.
// *************************************************************************** //

//  "ColorConvertU32ToFloat4_constexpr"
//
//      constexpr equivalent of the "ImGui::ColorConvertU32ToFloat4( ... )" function.
//      0xAABBGGRR      =>       (r,g,b,a)  ∈  [0,1]
//
[[nodiscard]]
inline constexpr ImVec4 ColorConvertU32ToFloat4_constexpr(const ImU32 c) noexcept
{
    constexpr float     inv255      = 1.0f / 255.0f;
    const float         r           = static_cast<float>((c >> IM_COL32_R_SHIFT) & 0xFFu) * inv255;
    const float         g           = static_cast<float>((c >> IM_COL32_G_SHIFT) & 0xFFu) * inv255;
    const float         b           = static_cast<float>((c >> IM_COL32_B_SHIFT) & 0xFFu) * inv255;
    const float         a           = static_cast<float>((c >> IM_COL32_A_SHIFT) & 0xFFu) * inv255;
    return {r, g, b, a};
}


//  "ColorConvertFloat4ToU32_constexpr"
//
[[nodiscard]]
inline constexpr ImU32 ColorConvertFloat4ToU32_constexpr(const ImVec4 & col) noexcept
{
    return (to_u8(col.x) << IM_COL32_R_SHIFT) |
           (to_u8(col.y) << IM_COL32_G_SHIFT) |
           (to_u8(col.z) << IM_COL32_B_SHIFT) |
           (to_u8(col.w) << IM_COL32_A_SHIFT);
}


//  "ColorConvertRGBtoHSV_constexpr"
//      constexpr equivalent of ImGui::ColorConvertRGBtoHSV(...)
//
inline constexpr void ColorConvertRGBtoHSV_constexpr(float r, float g, float b,
                                                     float& out_h, float& out_s, float& out_v) noexcept
{
    float K = 0.0f;
    if (g < b) { const float t = g; g = b; b = t; K = -1.0f; }
    if (r < g) { const float t = r; r = g; g = t; K = -2.0f / 6.0f - K; }

    const float mn      = (g < b ? g : b);
    const float chroma  = r - mn;
    const float denom_h = 6.0f * chroma + 1e-20f;

    out_h = _absf_(K + (g - b) / denom_h);
    out_s = chroma / (r + 1e-20f);
    out_v = r;
}


//  "ColorConvertHSVtoRGB_constexpr"
//      constexpr equivalent of ImGui::ColorConvertHSVtoRGB(...)
//
inline constexpr void ColorConvertHSVtoRGB_constexpr(float h, float s, float v,
                                                     float& out_r, float& out_g, float& out_b) noexcept
{
    if (s == 0.0f) { out_r = v; out_g = v; out_b = v; return; }  // gray

    const float h6 = _wrap01_(h) * 6.0f;          // fmod(h,1) * 6
    const int   i  = static_cast<int>(h6);        // 0..5
    const float f  = h6 - static_cast<float>(i);

    const float p = v * (1.0f - s);
    const float q = v * (1.0f - s * f);
    const float t = v * (1.0f - s * (1.0f - f));

    switch (i)
    {
        case 0:  out_r = v; out_g = t; out_b = p; break;
        case 1:  out_r = q; out_g = v; out_b = p; break;
        case 2:  out_r = p; out_g = v; out_b = t; break;
        case 3:  out_r = p; out_g = q; out_b = v; break;
        case 4:  out_r = t; out_g = p; out_b = v; break;
        default: out_r = v; out_g = p; out_b = q; break; // i==5
    }
}


//  "SetAlpha"
//      Uses BIT-WISE Operations to ASSIGN the alpha-value of an IMU32 color.
//
[[nodiscard]]
inline constexpr ImU32 SetAlpha(const ImU32 col, const ImU32 a) noexcept
    { return (col & ~IM_COL32_A_MASK) | (((a & 0xFFu) << IM_COL32_A_SHIFT)); }






// *************************************************************************** //
//              1.3.    NEW TEMPLATED COLOR FUNCTIONS.
// *************************************************************************** //

template <typename...>
inline constexpr bool dependent_false = false;


//  "ImGuiColorCast"
//      Cast ImGui color type T to U (ImVec4 <-> ImU32), constexpr-capable.
//
//     Usage:
//         ImU32    u   = ImGuiColorCast<ImU32>     ( ImVec4{1,0,0,1}   );
//         ImVec4   v   = ImGuiColorCast<ImVec4>    ( 0xFF0000FFu       );
//
template <class U, class T>
[[nodiscard]] inline constexpr U ImGuiColorCast(const T & c) noexcept
{
    //  CASE 0 :    Identity cast (ImVec4->ImVec4, ImU32->ImU32)
    if constexpr ( std::is_same_v<U, T> )
        { return c; }
    //
    //  CASE 1 :    Float4 -> U32
    else if constexpr ( std::is_same_v<U, ImU32> && std::is_same_v<T, ImVec4> )
        { return ColorConvertFloat4ToU32_constexpr(c); }
    //
    //  CASE 2 :    U32 -> Float4
    else if constexpr ( std::is_same_v<U, ImVec4> && std::is_same_v<T, ImU32> )
        { return ColorConvertU32ToFloat4_constexpr(c); }
    //
    //  DEFAULT :   ASSERTION FAILURE.
    else
        { static_assert(dependent_false<U, T>, "\"ImGuiColorCast\": unsupported color type conversion"); }
}




//      N E W    S T U F F . . .
// *************************************************************************** //
// *************************************************************************** //







//      S I M P L E    A S S E R T I O N    T E S T S . . .
// *************************************************************************** //
// *************************************************************************** //

static_assert   (   sizeof(ImU32) == 4,     "ImU32 is expected to be be 32-bit (4-bytes)"  );
//
static_assert   (   to_u8( 0.0f          )  ==  0    );
static_assert   (   to_u8( 1.0f          )  ==  255  );
static_assert   (   to_u8( 0.5f          )  ==  128  );
static_assert   (   to_u8( 254.0f/255.0f )  ==  254  );
static_assert   (   to_u8( 1.0f/255.0f   )  ==  1    );
//
static_assert   (
    ( IM_COL32_R_SHIFT % 8  == 0                )   &&  ( IM_COL32_G_SHIFT % 8  == 0                )       &&
    ( IM_COL32_B_SHIFT % 8  == 0                )   &&  ( IM_COL32_A_SHIFT % 8  == 0                )       &&
    ( IM_COL32_R_SHIFT      <= 24               )   &&  ( IM_COL32_G_SHIFT      <= 24               )       &&
    ( IM_COL32_B_SHIFT      <= 24               )   &&  ( IM_COL32_A_SHIFT      <= 24               )       &&
    ( IM_COL32_R_SHIFT      != IM_COL32_G_SHIFT )   &&  ( IM_COL32_R_SHIFT      != IM_COL32_B_SHIFT )       &&
    ( IM_COL32_R_SHIFT      != IM_COL32_A_SHIFT )   &&  ( IM_COL32_G_SHIFT      != IM_COL32_B_SHIFT )       &&
    ( IM_COL32_G_SHIFT      != IM_COL32_A_SHIFT )   &&  ( IM_COL32_B_SHIFT      != IM_COL32_A_SHIFT )
//
    , "IM_COL32_*_SHIFT must be a distinct permutation of {0,8,16,24}"
);



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "CONSTEXPR COLOR UTILs" ]].












// *************************************************************************** //
//
//
//
//      2.      MAIN COLOR UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//              2.1.    SHADE COMPUTATIONS.
// *************************************************************************** //

//  "compute_shade"
//
//      shade >  0 : => darker (percentage)          e.g.  0.20 :     => 20% darker
//      shade ≤  0 : => brighter (tint)              e.g. -0.15 :     => 15% brighter
//
[[nodiscard]]
inline ImVec4 compute_shade(const ImVec4 & color, const float shade = 0.0484f)
{
    constexpr float     MIN_FACTOR      = 0.0f;
    constexpr float     MAX_FACTOR      = 1.0f;
    const float         factor          = (shade >= 0.0f)
                                            ? (1.0f - std::clamp(shade, MIN_FACTOR, MAX_FACTOR))
                                            : (1.0f + std::clamp(shade, -MAX_FACTOR, MIN_FACTOR));

    return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);                 // preserve alpha
}

//  "compute_shade"
//
[[nodiscard]]
inline ImU32 compute_shade(const ImU32& color, const float shade = 0.0484f) noexcept
{
    const ImVec4    src     = ImGui::ColorConvertU32ToFloat4(color);
    const ImVec4    dst     = compute_shade(src, shade);
    return ImGui::ColorConvertFloat4ToU32(dst);
}

//  "compute_shade"
//      ImColor => ImColor   (wrapper around ImVec4)
//
[[nodiscard]]
inline ImColor compute_shade(const ImColor & color, const float shade = 0.0484f)
{
    const ImVec4    src     = color;               // ImColor => ImVec4 implicit
    const ImVec4    dst     = compute_shade(src, shade);
    return ImColor(dst);
}



// *************************************************************************** //
//              2.2.    TINT COMPUTATIONS.
// *************************************************************************** //

//  "compute_tint"
//      +0.25  =>  25% brighter     |   -0.20  =>  20% darker
//
inline ImVec4 compute_tint(const ImVec4 & color, const float tint = 0.0484f)
{
    constexpr float     MIN_FACTOR  = 0.0f;
    constexpr float     MAX_FACTOR  = 1.0f;
    float               factor      = (tint >= 0.0f)
                                        ? (1.0f + std::clamp(tint, MIN_FACTOR, MAX_FACTOR))   // brighten
                                        : (1.0f - std::clamp(-tint, MIN_FACTOR, MAX_FACTOR)); // darken

    return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);        // alpha unchanged
}

//  "compute_tint"
//
inline ImU32 compute_tint(const ImU32 & color, const float tint = 0.0484f)
{
    ImVec4      as_vec4     = ImGui::ColorConvertU32ToFloat4(color);
    ImVec4      tinted      = compute_tint(as_vec4, tint);
    return ImGui::ColorConvertFloat4ToU32(tinted);
}

//  "compute_tint"
//
inline ImColor compute_tint(const ImColor & color, const float tint = 0.0484f)
{
    ImVec4      as_vec4     = static_cast<ImVec4>(color);
    ImVec4      tinted      = compute_tint(as_vec4, tint);
    return ImColor(tinted);
}



// *************************************************************************** //
//              2.3.    COLOR-CONTRAST COMPARISONS.
// *************************************************************************** //

//  "ContrastType"
//
enum class ContrastType : uint8_t
{
      Relative = 0        // signed Michelson on relative luminance (linear sRGB)
    , Perceptual          // (planned) CIELAB L* signed difference, normalized
    , Michelson           // (planned) explicit Michelson; may alias Relative
    , Weber               // (planned)
    , APCA                // (planned) WCAG 3 candidate
    , COUNT
};



// *************************************************************************** //
//      "anon" |    INTERNAL NAMESPACE.
// *************************************************************************** //
namespace anon { //     BEGINNING NAMESPACE "anon"...


constexpr ContrastType      cv_DEFAULT_CONTRAST_METHOD      = ContrastType::Relative;   //  Default contrast model.


//  "srgb_to_linear"
//      sRGB (gamma) -> linear sRGB
//
[[nodiscard]] inline float srgb_to_linear(float c) noexcept {
    return (c <= 0.04045f) ? (c / 12.92f)
                           : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

//  "relative_luminance"
//      Relative luminance Y in [0,1] from ImVec4 (alpha ignored; RGB assumed sRGB 0..1)
//
[[nodiscard]] inline float relative_luminance(const ImVec4& v) noexcept {
    const float R = srgb_to_linear(std::clamp(v.x, 0.0f, 1.0f));
    const float G = srgb_to_linear(std::clamp(v.y, 0.0f, 1.0f));
    const float B = srgb_to_linear(std::clamp(v.z, 0.0f, 1.0f));
    return 0.2126f * R + 0.7152f * G + 0.0722f * B;
}

//  "contrast_signed_michelson"
//      Signed Michelson contrast on luminance, normalized to (-1,1).
//      Positive -> a is brighter; Negative -> a is darker; 0 -> equal luminance.
//
[[nodiscard]] inline float contrast_signed_michelson(const ImVec4& a, const ImVec4& b) noexcept {
    constexpr float EPS = 1e-6f;
    const float Ya  = relative_luminance(a);
    const float Yb  = relative_luminance(b);
    const float sum = Ya + Yb;
    if (sum <= EPS) return 0.0f;                         // both ~black
    const float diff = Ya - Yb;
    const float mag  = std::fabs(diff) / (sum + EPS);    // in [0,1)
    return (diff >= 0.0f) ? mag : -mag;
}


// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "anon".



//  "contrast_of"
//
//      PUBLIC API:
//          heterogeneous ImGui color types via your ImGuiColorCast
//          Returns signed, normalized contrast in (-1,1]; alpha is ignored.
//
template <class A, class B>
[[nodiscard]] inline float contrast_of(  const A & a
                                       , const B & b
                                       , ContrastType method = anon::cv_DEFAULT_CONTRAST_METHOD) noexcept
{
    assert( (method == ContrastType::Relative) || (method == ContrastType::Michelson) );     //  Only models that we have implemented right now.
    //
    const ImVec4    va          = ImGuiColorCast<ImVec4>(a); // cblib::utl::ImGuiColorCast
    const ImVec4    vb          = ImGuiColorCast<ImVec4>(b);
    float           contrast    = 0.0f;


    switch (method)
    {
        //      CASE 1 :    DEFAULT CONTRAST...
        case ContrastType::Relative:
        case ContrastType::Michelson: {                             //  until specialized, alias to Relative.
            contrast = anon::contrast_signed_michelson(va, vb);
            break;
        }
        //
        //      Placeholders for future implementations:
        case ContrastType::Perceptual   :                           //  CIELAB L* based (planned)
        case ContrastType::Weber        :                           //  Weber contrast (planned)
        case ContrastType::APCA         :                           //  APCA Lc (planned)
        default                         : {
            break;
        }
    }
    
    return contrast;
}





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "MAIN COLOR UTILs" ]].










// *************************************************************************** //
//
//
//
//      3.      COLORMAP COMPUTATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//              3.1.    PREVIOUS COLOR-MAP STUFF.
// *************************************************************************** //

//  "GetColormapSamples"
//
[[nodiscard]] inline std::vector<ImVec4> GetColormapSamples(const size_t M)
{
    std::vector<ImVec4>     colors(M);
    //ImPlotColormap  map     = ImPlot::GetCurrentColormap();
    
    for (size_t i = 0; i < M; ++i)
    {
        float t = (M > 1)   ? float(i) / float(M - 1)   : 0.0f;
        colors[i] = ImPlot::SampleColormap(t);
    }

    return colors;
}


//  "GetColorMapSamples"
//
[[nodiscard]] inline std::vector<ImVec4> GetColormapSamples(int M, const char * cmap)
{
    std::vector<ImVec4> cols;
    cols.reserve(M);

    // Activate the colormap so SampleColormap() uses it:
    ImPlot::PushColormap(cmap);

    for (int r = 0; r < M; r++) {
        float t = (M > 1)
                ? float(r) / float(M - 1)           //  0 => 1
                : 0.0f;                             //  single entry
        cols.push_back(ImPlot::SampleColormap(t));
    }

    ImPlot::PopColormap();
    return cols;
}


//  "GetColormapSamples"
//
[[nodiscard]] inline std::vector<ImVec4> GetColormapSamples(int M, ImPlotColormap map)
{
    std::vector<ImVec4> cols;
    cols.reserve(M);

    //  Activate the colormap so SampleColormap() uses it:
    ImPlot::PushColormap(map);

    for (int r = 0; r < M; r++)
    {
        float t = (M > 1)
                ? float(r) / float(M - 1)   //  0 -> 1
                : 0.0f;                     //  single entry
        cols.push_back(ImPlot::SampleColormap(t));
    }
    

    ImPlot::PopColormap();
    return cols;
}


//  "CreateTransparentColormap"
//
[[nodiscard]] inline ImPlotColormap CreateTransparentColormap(ImPlotColormap base_index, float alpha, const char* name = nullptr)
{
    const int               count       = ImPlot::GetColormapSize(base_index);
    std::vector<ImVec4>     colors;

    for (int i = 0; i < count; ++i)
    {
        ImVec4 c    = ImPlot::GetColormapColor(i, base_index);  // Use the public API
        c.w         = alpha; // Apply custom alpha
        colors.push_back(c);
    }

    if (!name)  { name = "TransparentCustom"; }
    return ImPlot::AddColormap( name, colors.data(), static_cast<int>(colors.size()) );
}



// *************************************************************************** //
//              3.2.    NEW COLOR-MAP STUFF.
// *************************************************************************** //

// "GetHDRAdjacentColormapSamples"
//
//      - Samples the SAME M colors from `map` as GetColormapSamples()
//      - Reorders them so that consecutive entries have HIGH pairwise contrast,
//        using your cblib::utl::contrast_of metric (default: signed Michelson on luminance).
//      - Heuristic: start with the pair of colors that maximizes |contrast|,
//        then greedily grow the sequence by appending the remaining color that best
//        improves the ends, prioritizing an even distribution via max-min selection.
//
//      COMPLEXITY:     O(M^2)      (pairwise contrast matrix + greedy growth).
//
[[nodiscard]] inline std::vector<ImVec4>
GetHDRAdjacentColormapSamples(  const int               M
                              , const ImPlotColormap    map
                              , const ContrastType      method = anon::cv_DEFAULT_CONTRAST_METHOD) noexcept
{
    assert(M > 0);
    std::vector<ImVec4>     cols;
    cols.reserve(M);
    
    if (M == 1) {
        ImPlot::PushColormap(map);
        cols.push_back(ImPlot::SampleColormap(0.0f));
        ImPlot::PopColormap();
        return cols;
    }

    //      1.      Sample uniformly [0,1] (endpoint inclusive)
    ImPlot::PushColormap(map);
    {
        const float     inv     = 1.0f / float(M - 1);
        for (int r = 0; r < M; ++r) {
            const float t = float(r) * inv;
            cols.push_back(ImPlot::SampleColormap(t));
        }
    }
    ImPlot::PopColormap();


    if (M == 2)     { return cols; }



    //      2.      PRECOMPUTE MATRIX OF |contrast|  [ symmetric, zero diagonal ]...
    //
    std::vector<float>      C           (std::size_t(M) * std::size_t(M), 0.0f);
    auto                    cm          = [&](int i, int j) -> float& { return C[std::size_t(i) * M + j]; };

    for (int i = 0; i < M; ++i)
    {
        for (int j = i + 1; j < M; ++j)
        {
            const float v = std::fabs(contrast_of(cols[i], cols[j], method));
            cm(i, j) = v;
            cm(j, i) = v;
        }
    }

    //      3.      SEED WITH THE PAIR OF MAXIMUM CONTRASTING COLORS...
    int a = 0,   b = 1;
    {
        float best = -1.0f;
        for (int i = 0; i < M; ++i)
        {
            for (int j = i + 1; j < M; ++j)
                if (cm(i, j) > best) { best = cm(i, j); a = i; b = j; }
        }
    }

    std::vector<char>   used    (std::size_t(M), 0);
    used[a] = used[b] = 1;

    std::deque<int>     order;
    order.push_back(a);
    order.push_back(b);


    //      4.      GREEDY---GROWTH.
    //                  At each step, pick k that best improves either end.
    //                  We prefer the candidate that maximizes min(contrast_to_front, contrast_to_back)
    //                  to distribute contrast more evenly; break ties by maximizing the max().
    //
    while ( (int)order.size() < M )
    {
        int         best_k              = -1;
        bool        attach_front        = false;
        float       best_min            = -1.0f;
        float       best_max            = -1.0f;
        const int   L                   = order.front();
        const int   R                   = order.back();

        for (int k = 0; k < M; ++k)
        {
            if (used[k]) continue;

            const float     cf      = cm(k, L);
            const float     cb      = cm(R, k);
            const float     m0      = std::min(cf, cb);
            const float     M0      = std::max(cf, cb);

            if (m0 > best_min || (m0 == best_min && M0 > best_max))
            {
                best_min    = m0;
                best_max    = M0;
                best_k      = k;
                attach_front = (cf >= cb);  // attach to side with larger contrast
            }
        }

        if (best_k < 0)     { break; }  // safety

        if (attach_front)   { order.push_front(best_k); }
        else                { order.push_back (best_k); }

        used[best_k] = 1;
    }
    

    //      5.      MATERIALIZE THE REORDERED COLORS...
    std::vector<ImVec4>     out;
    out.reserve(M);
    for (int idx : order)   { out.push_back(cols[idx]); }
    
    return out;
}




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "CMAP UTILs." ]].










// *************************************************************************** //
//
//
//
//      4.      IMGUI BITFLAG-BASED INLINE UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "SetFlag"
//
template<typename T>
inline void SetFlag(T * flags, T flags_value)
{
    bool all_on = (*flags & flags_value) == flags_value;
    bool any_on = (*flags & flags_value) != 0;
    //  bool pressed;
    
    
    //  if (!all_on && any_on)
    //  {
    //      ImGuiContext& g = *GImGui;
    //      g.NextItemData.ItemFlags |= ImGuiItemFlags_MixedValue;
    //      pressed = Checkbox(label, &all_on);
    //  }
    //  else
    //  { pressed = Checkbox(label, &all_on); }
    

    if (all_on)
        *flags |= flags_value;
    else
        *flags &= ~flags_value;

    
    return;
}


//  "RemoveFlag"
//
template<typename T>
inline void RemoveFlag(T * flags, T flags_value)
{
    bool all_on = (*flags & flags_value) == flags_value;
    bool any_on = (*flags & flags_value) != 0;
    //  bool pressed;
    
    
    //  if (!all_on && any_on)
    //  {
    //      ImGuiContext& g = *GImGui;
    //      g.NextItemData.ItemFlags |= ImGuiItemFlags_MixedValue;
    //      pressed = Checkbox(label, &all_on);
    //  }
    //  else
    //  { pressed = Checkbox(label, &all_on); }
    
    if (all_on)
        *flags |= flags_value;
    else
        *flags &= ~flags_value;
    
    return;
}





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 4.  "BITFLAG FUNCTIONS." ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "utl" NAMESPACE.








// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_IMGUI_EXTENSIONS_UTILITY_H  //
