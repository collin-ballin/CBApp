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



//      1.0.    "COLOR"-BASED INLINE UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ColorConvertU32ToFloat4_constexpr"
//      constexpr equivalent of the "ImGui::ColorConvertU32ToFloat4( ... )" function.
//      0xAABBGGRR      →       (r,g,b,a)  ∈  [0,1]
//
inline constexpr ImVec4 ColorConvertU32ToFloat4_constexpr(const ImU32 c) noexcept
{
    constexpr float inv255 = 1.0f / 255.0f;
    return {
        static_cast<float>(( c        ) & 0xFF) * inv255,   // R
        static_cast<float>(( c >>  8 )  & 0xFF) * inv255,   // G
        static_cast<float>(( c >> 16 )  & 0xFF) * inv255,   // B
        static_cast<float>(( c >> 24 )  & 0xFF) * inv255    // A
    };
}


//  "ColorConvertFloat4ToU32_constexpr"
//
[[nodiscard]]
constexpr ImU32 ColorConvertFloat4ToU32_constexpr(const ImVec4 & col) noexcept
{
    constexpr auto to_u8 = [](float f) constexpr -> ImU32
    {
        return static_cast<ImU32>(
            f <= 0.0f ? 0u :
            f >= 1.0f ? 255u :
            f * 255.0f + 0.5f);          // round-to-nearest
    };

    return (to_u8(col.x) << IM_COL32_R_SHIFT) |
           (to_u8(col.y) << IM_COL32_G_SHIFT) |
           (to_u8(col.z) << IM_COL32_B_SHIFT) |
           (to_u8(col.w) << IM_COL32_A_SHIFT);
}


//  "compute_shade"
//
//      shade >  0 : → darker (percentage)          e.g. 0.20 :     → 20 % darker
//      shade ≤  0 : → brighter (tint)              e.g.-0.15 :     → 15 % brighter
//
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
inline ImU32 compute_shade(const ImU32 & color, const float shade = 0.0484f) {
    const ImVec4    src     = ImGui::ColorConvertU32ToFloat4(color);
    const ImVec4    dst     = compute_shade(src, shade);
    return ImGui::ColorConvertFloat4ToU32(dst);
}

//  "compute_shade"
//      ImColor → ImColor   (wrapper around ImVec4)
//
inline ImColor compute_shade(const ImColor & color, const float shade = 0.0484f) {
    const ImVec4    src     = color;               // ImColor → ImVec4 implicit
    const ImVec4    dst     = compute_shade(src, shade);
    return ImColor(dst);
}






//  "compute_tint"
//      +0.25 → 25 % brighter   |   -0.20 → 20 % darker
//
inline ImVec4 compute_tint(const ImVec4 & color, const float tint = 0.0484f) {
    constexpr float     MIN_FACTOR  = 0.0f;
    constexpr float     MAX_FACTOR  = 1.0f;
    float               factor      = (tint >= 0.0f)
                                        ? (1.0f + std::clamp(tint, MIN_FACTOR, MAX_FACTOR))   // brighten
                                        : (1.0f - std::clamp(-tint, MIN_FACTOR, MAX_FACTOR)); // darken

    return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);        // alpha unchanged
}

//  "compute_tint"
//
inline ImU32 compute_tint(const ImU32 & color, const float tint = 0.0484f) {
    ImVec4      as_vec4     = ImGui::ColorConvertU32ToFloat4(color);
    ImVec4      tinted      = compute_tint(as_vec4, tint);
    return ImGui::ColorConvertFloat4ToU32(tinted);
}

//  "compute_tint"
//
inline ImColor compute_tint(const ImColor & color, const float tint = 0.0484f) {
    ImVec4      as_vec4     = static_cast<ImVec4>(color);
    ImVec4      tinted      = compute_tint(as_vec4, tint);
    return ImColor(tinted);
}



//
//
// *************************************************************************** //
// *************************************************************************** //   END "COLOR FUNCTIONS".






// *************************************************************************** //
//
//
//      2.0.    IMGUI BITFLAG-BASED INLINE UTILITY FUNCTIONS...
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
