/***********************************************************************************
*
*       ********************************************************************
*       ****                _ J S O N . H  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 12, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_IMGUI_EXTENSIONS_JSON_H
#define _CBLIB_IMGUI_EXTENSIONS_JSON_H 1

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


//      "ImVec2"            SERIALIZER...
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












namespace cblib { //     BEGINNING NAMESPACE "cblib"...
// *************************************************************************** //
// *************************************************************************** //



//  "SchemaVersion"
//
struct SchemaVersion {
// *************************************************************************** //
// *************************************************************************** //
//
//
    using               value_type          = uint8_t;
    value_type          major;                              //       {0};
    value_type          minor;                              //       {0};
    
    
    
    //  Overloaded Operators.
    friend constexpr bool                           operator ==                         (const SchemaVersion & , const SchemaVersion & )    = default;
    friend constexpr auto                           operator <=>                        (const SchemaVersion & , const SchemaVersion & )    = default;

    
    //  "is_backward_compatible_with"
    [[nodiscard]] constexpr bool                    is_backward_compatible_with         (const SchemaVersion & current) const
        { return ( (major == current.major)  &&  (minor <= current.minor) );  }
    
    
    //  "to_string"
    [[nodiscard]] std::string                       to_string                           (void) const
        { return std::to_string(major) + "." + std::to_string(minor); }
    
    
    //  "pack"  and  "unpack"
  	[[nodiscard]] constexpr std::uint16_t           pack                                (void) const noexcept
        { return (std::uint16_t(major) << 8) | std::uint16_t(minor); }
	[[nodiscard]] static constexpr SchemaVersion    unpack                              (std::uint16_t v) noexcept
        { return { value_type(v >> 8), value_type(v & 0xFF) }; }
    
  
  
// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "SchemaVersion" INLINE CLASS DEFINITION.



// *************************************************************************** //
// *************************************************************************** //
//
    static_assert   (   std::is_trivially_copyable_v<SchemaVersion>     );
    static_assert   (   std::is_aggregate_v<SchemaVersion>              );      //  keep it an aggregate (good for NTTPs)
    static_assert   (   sizeof(SchemaVersion) == 2                      );      //  locks layout you expect
//
// *************************************************************************** //
// *************************************************************************** //



//  "to_json"
//      nlohmann::json adapters (object form + legacy fallbacks).
//
inline void to_json(nlohmann::json & j, const SchemaVersion & v)
    { j = nlohmann::json{{"major", v.major}, {"minor", v.minor}}; }


//  "from_json"
//
inline void from_json(const nlohmann::json & j, SchemaVersion & v)
{
    using           json            = nlohmann::json;
    using           value_type      = SchemaVersion::value_type;
    
    
    if ( j.is_object() )
    {
        std::uint32_t   M   = 0;
        std::uint32_t   m   = 0;
        
        j.at("major").get_to(M);
        
        m                   = j.value("minor", 0u);
        
        if ( M > 255u || m > 255u )     { throw json::out_of_range::create(0, "version out of range", &j); }
        
        
        v.major             = static_cast<value_type>(M);
        v.minor             = static_cast<value_type>(m);
    }
    //
    //
    else if ( j.is_number_unsigned() )  // legacy: 4  -> {4,0}
    {
        const auto      M    = j.get<std::uint32_t>();
        if ( M > 255u )     { throw json::out_of_range::create(0, "version out of range", &j); }
        
        v.major             = static_cast<value_type>(M);
        v.minor             = 0;
    }
    //
    //
    else if ( j.is_string() )// legacy: "4.1"
    {
        const std::string   s       = j.get<std::string>();
        const auto          dot     = s.find('.');
        const auto          M       = std::stoul(s.substr(0, dot));
        const auto          m       = (dot == std::string::npos) ? 0ul : std::stoul(s.substr(dot + 1));
        
        if ( M > 255u || m > 255u )     { throw json::out_of_range::create(0, "version out of range", &j); }
        
        v.major = static_cast<value_type>(M);
        v.minor = static_cast<value_type>(m);
    }
    //
    //
    else
    {
        throw json::type_error::create(302, "invalid 'version' type", &j);
    }
    
    
    return;
}


//  "write_version_field"
//      Convenience helpers for a top-level "version" field ----
//
inline void write_version_field(nlohmann::json & j, const SchemaVersion & v, std::string_view key="version")
{
    j[std::string(key)]     = v;    // uses to_json
    return;
}


//  "read_version_field"
//
inline SchemaVersion read_version_field(const nlohmann::json & j, std::string_view key="version")
{
    using           json    = nlohmann::json;
    const auto      it      = j.find( std::string(key) );
    
    if ( it == j.end() )    { throw json::out_of_range::create(0, "version out of range", &j); }
    
    
    return ( it->get<SchemaVersion>() ); // uses from_json with fallbacks
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cblib" NAMESPACE.

























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
#endif  //  _CBLIB_IMGUI_EXTENSIONS_JSON_H  //
