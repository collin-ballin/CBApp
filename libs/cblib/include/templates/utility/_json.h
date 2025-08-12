/***********************************************************************************
*
*       ********************************************************************
*       ****                _ J S O N . H  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      August 12, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_UTILITY_JSON_H
#define _CBLIB_UTILITY_JSON_H 1

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












// *************************************************************************** //
//
//
//
//  ?.?     JSON SERIALIZERS...
// *************************************************************************** //
// *************************************************************************** //

namespace nlohmann { //     BEGINNING NAMESPACE "nlohmann"...
// *************************************************************************** //
// *************************************************************************** //

template<>
struct adl_serializer<ImVec2>
{
    static void to_json(json & j, const ImVec2 & v)
    {
        j = json::array({ v.x, v.y });          // -> [x, y]
    }

    static void from_json(const json & j, ImVec2 & v)
    {
        if ( !j.is_array() || j.size() != 2 )
        { throw std::runtime_error("ImVec2 expects JSON array [x, y]"); }

        v.x     = j[0].get<float>();
        v.y     = j[1].get<float>();
    }
};






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "nlohmann" NAMESPACE.












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
#endif  //  _CBLIB_UTILITY_JSON_H  //
