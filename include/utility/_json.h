/***********************************************************************************
*
*       ********************************************************************
*       ****                _ J S O N . H  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 29, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_UTILITY_JSON_H
#define _CBAPP_UTILITY_JSON_H      1

#include <stdio.h>
#include <vector>
#include <math.h>
#include <cmath>

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












namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //


//  "has_from_json"
//
/// @struct     has_from_json
/// @brief      Detect if a type has a valid "from_json" serializer function.
/// @note       Template type-trait style function to determine if a given type has a visible "from_json"
///             serializer function as specified within the "nlohmann JSON for Modern C++" library.
///             USAGE:  static_assert( has_from_json<MyType>::value, "Type \"MyType\" does NOT have a visible \"from_json\" serializer." );
///
/// @tparam     T, the type to query serializer status
///
///
template<typename T, typename = void>
struct has_from_json : std::false_type {};

//  "has_from_json"
//
template<typename T>
struct has_from_json<
    T,
    std::void_t< decltype(
        nlohmann::adl_serializer<T>::from_json(
            std::declval<const nlohmann::json &>(),
            std::declval<T &>() )
    ) >
    > : std::true_type
{/*  struct body  */};




//  "has_to_json"
//
/// @struct     has_to_json
/// @brief      Detect if a type has a valid "has_json" serializer function.
/// @note       Template type-trait style function to determine if a given type has a visible "to_json"
///             serializer function as specified within the "nlohmann JSON for Modern C++" library.
///             USAGE:  static_assert( has_to_json<MyType>::value, "Type \"MyType\" does NOT have a visible \"to_json\" serializer." );
///
/// @tparam     T, the type to query serializer status
///
template<typename T, typename = void>
struct has_to_json : std::false_type { };

//  "has_to_json"
//
template<typename T>
struct has_to_json<
    T,
    std::void_t<decltype(                       //  expression must be well‑formed
        nlohmann::adl_serializer<T>::to_json(
            std::declval<nlohmann::json &>(),           //  first arg        : json lvalue
            std::declval<const T&>() )                  //  second arg       : const T &
    ) >
    > : std::true_type
{/*  struct body  */};




// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb::utl" NAMESPACE.






#endif      //  _CBAPP_UTILITY_JSON_H  //
// *************************************************************************** //
// *************************************************************************** //  END.
