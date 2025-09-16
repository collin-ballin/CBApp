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



namespace cblib { namespace utl {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      CONSTEXPR COLOR UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "to_u8"
[[nodiscard]]
inline constexpr ImU32 to_u8(float f) {
    return static_cast<ImU32>(
                (f <= 0.0f)
                    ? 0u
                    : (f >= 1.0f)
                        ? 255u
                        : f * 255.0f + 0.5f);          // round-to-nearest
}


//  "ColorConvertU32ToFloat4_constexpr"
//
//      constexpr equivalent of the "ImGui::ColorConvertU32ToFloat4( ... )" function.
//      0xAABBGGRR      →       (r,g,b,a)  ∈  [0,1]
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


//  "SetAlpha"
//      Uses BIT-WISE Operations to ASSIGN the alpha-value of an IMU32 color.
//
[[nodiscard]]
inline constexpr ImU32 SetAlpha(const ImU32 col, const ImU32 a) noexcept
    { return (col & ~IM_COL32_A_MASK) | (((a & 0xFFu) << IM_COL32_A_SHIFT)); }






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
// *************************************************************************** //   END "CONSTEXPR COLORS".






// *************************************************************************** //
//
//
//
//      2.      MAIN COLOR UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "compute_shade"
//
//      shade >  0 : → darker (percentage)          e.g. 0.20 :     → 20 % darker
//      shade ≤  0 : → brighter (tint)              e.g.-0.15 :     → 15 % brighter
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
//      ImColor → ImColor   (wrapper around ImVec4)
//
[[nodiscard]]
inline ImColor compute_shade(const ImColor & color, const float shade = 0.0484f)
{
    const ImVec4    src     = color;               // ImColor → ImVec4 implicit
    const ImVec4    dst     = compute_shade(src, shade);
    return ImColor(dst);
}






//  "compute_tint"
//      +0.25 → 25 % brighter   |   -0.20 → 20 % darker
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN COLOR FUNCTIONS".






// *************************************************************************** //
//
//
//
//      3.      IMGUI BITFLAG-BASED INLINE UTILITY FUNCTIONS...
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
// *************************************************************************** //   END "BITFLAG" FUNCTIONS.












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
