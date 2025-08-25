/***********************************************************************************
*
*       ********************************************************************
*       ****         _ S E R I A L I Z E R S . H  ____  F I L E         ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 24, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_IMGUI_JSON_SERIALIZERS_H
#define _CBLIB_IMGUI_JSON_SERIALIZERS_H 1

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
//  # include "templates/imgui_extensions/json/_helpers.h"
//
#include "json.hpp"
#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers







//      1.      STATIC INLINE HELPERS...
// *************************************************************************** //
// *************************************************************************** //

namespace cblib { namespace misc {   //     BEGINNING NAMESPACE "cblib::json"...
// *************************************************************************** //
// *************************************************************************** //

//  COMPILE-TIME FLAG...
constexpr bool                          kBGRA                   = ( (IM_COL32_R_SHIFT == 16)  &&  (IM_COL32_B_SHIFT == 0) );


//  FLOAT TYPE...
//
//      "to_byte"
static inline constexpr unsigned        to_byte                 (float f)
{ return static_cast<unsigned>(std::clamp(f, 0.0f, 1.0f) * 255.0f + 0.5f); }
//
//      "to_float"      -- Inverse.
static inline constexpr float           to_float                (unsigned b)    { return b / 255.0f; }



//  4-COMPONENT FLOAT...
//
//      "rgba_to_hex"   -- 4-component float -> “RRGGBBAA” (always eight hex chars, uppercase).
static inline std::string               rgba_to_hex             (float r, float g, float b, float a)
{
    constexpr size_t        N           = 4ULL;
    static const char *     dig         = "0123456789ABCDEF";
    //
    unsigned                chan [N]    = { to_byte(r), to_byte(g), to_byte(b), to_byte(a) };
    std::string             out         ( 8,    '0' );
    //
    std::array<unsigned, N> idx         = {   };            //      Order bytes according to ImGui packing
    if constexpr ( kBGRA )      { idx = { 2,1,0,3 };    }   //          { B,G,R,A }.
    else                        { idx = { 0,1,2,3 };    }   //          { R,G,B,A }.


    for (int i = 0; i < static_cast<int>( N ); ++i)
    {
        unsigned    v       = chan[ idx[i] ];
        out [ i*2 ]         = dig[ v >> 4 ];
        out [ i*2 + 1 ]     = dig[ v & 15 ];
    }
    
    return out;
}


//      "hex_to_rgba"
//          Accept “RRGGBBAA” or “#RRGGBBAA”.  Returns array<4,float> RGBA in 0–1.
//
static inline std::array<float, 4>      hex_to_rgba             (std::string s)
{
    constexpr size_t            N           = 4ULL;
    std::array<float, N>        rgba        = {   };
    unsigned                    bytes [N]   = {   };
    //
    std::array<unsigned, N>     idx         = {   };            //  Map back to RGBA irrespective of packed order.
    if constexpr ( kBGRA )      { idx = { 2, 1, 0, 3 }; }       //          { B,G,R,A }.
    else                        { idx = { 0, 1, 2, 3 }; }       //          { R,G,B,A }.
    //
    auto                        hex         = [](char c) -> unsigned
    {
        c   = std::toupper( static_cast<unsigned char>( c ) );
        if ( std::isdigit(c) )                  { return (c - '0');         }
        if ( (c >= 'A')  &&  (c <= 'F') )       { return (c - 'A' + 10);    }
        
        throw std::runtime_error("invalid hex digit");
    };

    if ( !s.empty()  &&  (s.front() == '#') )       { s.erase(0, 1); }
    if ( s.size() != 8 )                            { throw std::runtime_error("hex_to_rgba expects 8 hex chars"); }



    for (int i = 0;i < static_cast<int>(N) ;++i) {
        bytes[i]        = ( (hex( s[i*2]) << 4) | hex(s[ i*2 + 1 ]) );
    }
    for (int i = 0; i < static_cast<int>(N); ++i) {
        rgba[ idx[i] ]  = to_float(bytes[i]);
    }
    
    return rgba;
}




// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib::json" NAMESPACE.












// *************************************************************************** //
//
//
//
//      ?.?     JSON SERIALIZERS...
// *************************************************************************** //
// *************************************************************************** //

namespace nlohmann { //     BEGINNING NAMESPACE "nlohmann"...
// *************************************************************************** //
// *************************************************************************** //



//      "ImVec2"                SERIALIZER...
//
//          - WRITES :  [x, y]                          (float array, canonical)
//          - READS  :  [x, y],
//              *OR*    { "x": ..., "y": ... }          (tolerant).
//
template<>
struct adl_serializer<ImVec2>
{
    static void                 to_json                 (json & j, const ImVec2 & v)
    {
        j = { v.x, v.y };
        return;
    }

    static void                 from_json               (const json & j, ImVec2 & v)
    {
        if ( (j.is_array())  &&  (j.size() == 2) ) {
            v.x     = j[0].get<float>();
            v.y     = j[1].get<float>();
            return;
        }
        if ( j.is_object() ) {
            v.x     = j.value("x", 0.0f);
            v.y     = j.value("y", 0.0f);
            return;
        }
        
        throw std::runtime_error("ImVec2 expects JSON array [x, y]");
        return;
    }
};




//      "ImVec4"                SERIALIZER...
//
template<>
struct adl_serializer<ImVec4>
{
    static void                 to_json                 (json & j, ImVec4 const & v)
    {
        j = { v.x, v.y, v.z, v.w };
        return;
    }

    static void                 from_json               (json const & j, ImVec4 & v)
    {
        if ( j.is_array()  &&  (j.size() == 4) ) {
            v = { j[0].get<float>(), j[1].get<float>(),
                  j[2].get<float>(), j[3].get<float>() };
            return;
        }
        if ( j.is_object() ) {
            v.x     = j.value("x", 0.0f);
            v.y     = j.value("y", 0.0f);
            v.z     = j.value("z", 0.0f);
            v.w     = j.value("w", 0.0f);
            return;
        }
        throw std::runtime_error("ImVec4 expects [r,g,b,a] or {x/y/z/w}");
        return;
    }
};


//      "ImColor"               SERIALIZER...
//
//          - WRITES :  "RRGGBBAA"                      (8-char hex string, canonical).
//          - READS  :  [r,g,b,a] float array           (for tolerance).
//
template<>
struct adl_serializer<ImColor>
{
    
    static void                 to_json                 (json & j, ImColor const & c)
    {
        using       namespace       cblib::misc;
        ImVec4      v               = c.Value;
        j                           = rgba_to_hex(v.x, v.y, v.z, v.w);   // e.g. "FFAA3380"
    }

    static void                 from_json               (json const & j, ImColor & c)
    {
        using       namespace       cblib::misc;
        
        if ( j.is_string() )
        {
            auto    rgba    = hex_to_rgba(j.get<std::string>());
            c               = ImColor(rgba[0], rgba[1], rgba[2], rgba[3]);
            return;
        }
        if ( j.is_array()  &&  (j.size() == 4) ) {
            c = ImColor(j[0].get<float>(), j[1].get<float>(),
                        j[2].get<float>(), j[3].get<float>());
            return;
        }
        throw std::runtime_error("ImColor expects \"RRGGBBAA\" or [r,g,b,a]");
    }
};




// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "nlohmann" NAMESPACE.




//  Are there additional ImGui data types that it would behoove me to specify JSON serializer functions for right now in order to ease some programming tasks that may arise in the future development of this project?
























namespace cblib { namespace utl {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "utl" NAMESPACE.








// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_IMGUI_JSON_SERIALIZERS_H  //
