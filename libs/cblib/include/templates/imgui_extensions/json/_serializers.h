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
template<>
struct adl_serializer<ImVec2>
{
    static void                 to_json                 (json & j, const ImVec2 & v)
    {
        j = json::array({ v.x, v.y });          // -> [x, y]
    }

    static void                 from_json               (const json & j, ImVec2 & v)
    {
        if ( !j.is_array() || j.size() != 2 )
        { throw std::runtime_error("ImVec2 expects JSON array [x, y]"); }

        v.x     = j[0].get<float>();
        v.y     = j[1].get<float>();
    }
};


//      "ImVec4"                SERIALIZER...
//
//  template<>
//  struct adl_serializer<ImVec4>
//  {
//      static void                 to_json                 (json & j, ImVec4 const & v)
//      {
//          j = {v.x, v.y, v.z, v.w};
//      }
//
//      static void                 from_json               (json const & j, ImVec4 & v)
//      {
//          if ( (j.is_array())  &&  (j.size() == 4) )
//          {
//              v = {
//                  j[0].get<float>(), j[1].get<float>(),
//                  j[2].get<float>(), j[3].get<float>()
//              };
//              return;
//          }
//          v.x = j.value("x",0.f); v.y=j.value("y",0.f); v.z=j.value("z",0.f); v.w=j.value("w",0.f);
//      }
//  };


//      "ImColor"               SERIALIZER...
//
//  template<>
//  struct adl_serializer<ImColor>
//  {
//      static void                 to_json                 (json & j, ImColor const & c)       //  uses ImVec4 serializer.
//      {
//          j = c.Value;
//      }
//
//      static void                 from_json               (json const & j, ImColor & c)
//      {
//          ImVec4 v; from_json(j, v); c = v;
//      }
//  };




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
